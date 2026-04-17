#include "ota.h"
#include "common/pinouts.h"
#include "ArduinoJson.h"
#include "EEPROM.h"
#include "SD.h"
#include "utility/SdFat.h"
#include "utility/Sd2Card.h"
#include <stm32f1xx.h>

// Flash controller unlock keys (RM0008 §3.3.2)
#define OTA_FLASH_KEY1    0x45670123UL
#define OTA_FLASH_KEY2    0xCDEF89ABUL

// FLASH_SR bits
#define OTA_SR_BSY        (1UL << 0)
#define OTA_SR_EOP        (1UL << 5)

// FLASH_CR bits
#define OTA_CR_PG         (1UL << 0)
#define OTA_CR_PER        (1UL << 1)
#define OTA_CR_STRT       (1UL << 6)
#define OTA_CR_LOCK       (1UL << 7)

// ─── Raw SPI / SD helpers for the RAM trampoline ─────────────────────────────
//
// These execute from SRAM (inlined into ota_ram_apply via always_inline).
// All register addresses are hardcoded — no global/library state is accessed.
//
// SPI1 on STM32F103 (APB2, base 0x40013000):
//   SR offset 0x08,  DR offset 0x0C
//   SR bit 1 = TXE (transmit buffer empty)
//   SR bit 0 = RXNE (receive buffer not empty)
//
// GPIOA (APB2, base 0x40010800):
//   BSRR offset 0x10 (set bits),  BRR offset 0x14 (reset bits)
//   CS pin = PA15 (SYSTEM_SD_CS_PIN)

#define TRAM_SPI1_SR    (*(volatile uint32_t*)0x40013008UL)
#define TRAM_SPI1_DR    (*(volatile uint32_t*)0x4001300CUL)
#define TRAM_GPIOA_BSRR (*(volatile uint32_t*)0x40010810UL)
#define TRAM_GPIOA_BRR  (*(volatile uint32_t*)0x40010814UL)

#define TRAM_CS_LOW()   TRAM_GPIOA_BRR  = (1UL << 15)   // assert   CS → PA15 low
#define TRAM_CS_HIGH()  TRAM_GPIOA_BSRR = (1UL << 15)   // deassert CS → PA15 high

// Exchange one byte over SPI1.
__attribute__((always_inline))
static inline uint8_t tram_spi_byte(uint8_t b)
{
    while (!(TRAM_SPI1_SR & (1UL << 1)));   // wait TXE
    TRAM_SPI1_DR = b;
    while (!(TRAM_SPI1_SR & (1UL << 0)));   // wait RXNE
    return (uint8_t)TRAM_SPI1_DR;
}

// Read one 512-byte sector from the SD card using CMD17.
// block  — absolute SD block number
// sdhc   — 1 for SDHC/SDXC (block addr), 0 for SDSC (byte addr = block * 512)
// buf    — must point to a 512-byte buffer in SRAM
__attribute__((always_inline))
static inline void tram_sd_read_sector(uint32_t block, uint8_t sdhc, uint8_t* buf)
{
    uint32_t addr = sdhc ? block : (block * 512UL);

    TRAM_CS_LOW();

    // CMD17: READ_SINGLE_BLOCK — byte 0 = 0x40 | 17 = 0x51
    tram_spi_byte(0x51);
    tram_spi_byte((uint8_t)(addr >> 24));
    tram_spi_byte((uint8_t)(addr >> 16));
    tram_spi_byte((uint8_t)(addr >>  8));
    tram_spi_byte((uint8_t) addr);
    tram_spi_byte(0xFF);                    // CRC byte (ignored in SPI mode)

    // Wait for R1 response (should be 0x00 = no error)
    uint8_t r;
    do { r = tram_spi_byte(0xFF); } while (r == 0xFF);

    // Wait for data start token 0xFE
    do { r = tram_spi_byte(0xFF); } while (r != 0xFE);

    // Read 512 data bytes
    for (uint16_t i = 0; i < 512; i++) {
        buf[i] = tram_spi_byte(0xFF);
    }

    // Discard 2 CRC bytes
    tram_spi_byte(0xFF);
    tram_spi_byte(0xFF);

    TRAM_CS_HIGH();
    tram_spi_byte(0xFF);    // 8 extra clocks so the SD card deselects cleanly
}

// ─── RAM trampoline ──────────────────────────────────────────────────────────
//
// Placed in the .RamFunc section so the linker script (ldscript.ld line 133)
// copies it into SRAM alongside .data at startup.  It therefore runs entirely
// from SRAM and can safely erase every page of application flash.
//
// sectors      — array of absolute SD block numbers (must be in SRAM)
// sector_count — number of valid entries in sectors[]
// fw_size      — exact firmware byte count (used to compute pages to erase)
// sdhc         — 1 = SDHC addressing, 0 = SDSC byte addressing
//
// NEVER RETURNS — ends with NVIC_SystemReset().
//
__attribute__((section(".RamFunc"), noinline, used))
static void ota_ram_apply(const uint32_t* sectors, uint16_t sector_count,
                          uint32_t fw_size, uint8_t sdhc)
{
    __disable_irq();

    // Unlock flash controller
    FLASH->KEYR = OTA_FLASH_KEY1;
    FLASH->KEYR = OTA_FLASH_KEY2;

    // 512-byte I/O buffer on the stack (stack is in SRAM — untouched by flash ops)
    uint8_t buf[512];

    uint16_t pages = (uint16_t)((fw_size + OTA_PAGE_SIZE - 1) / OTA_PAGE_SIZE);

    for (uint16_t page = 0; page < pages; page++) {

        uint32_t flash_addr = OTA_APP_ADDR + (uint32_t)page * OTA_PAGE_SIZE;
        uint16_t sec0 = (uint16_t)(page * 2);       // first  512-byte half of this 1 KB page
        uint16_t sec1 = (uint16_t)(page * 2 + 1);   // second 512-byte half

        // ── Erase this 1 KB flash page ────────────────────────────────────
        while (FLASH->SR & OTA_SR_BSY);
        FLASH->CR  = OTA_CR_PER;
        FLASH->AR  = flash_addr;
        FLASH->CR |= OTA_CR_STRT;
        while (FLASH->SR & OTA_SR_BSY);
        FLASH->SR  = OTA_SR_EOP;
        FLASH->CR  = 0;

        // ── Write first 512 bytes ─────────────────────────────────────────
        if (sec0 < sector_count) {
            tram_sd_read_sector(sectors[sec0], sdhc, buf);
        } else {
            for (uint16_t i = 0; i < 512; i++) buf[i] = 0xFF;
        }

        FLASH->CR = OTA_CR_PG;
        for (uint16_t i = 0; i < 512; i += 2) {
            uint16_t hw = (uint16_t)buf[i] | ((uint16_t)buf[i + 1] << 8);
            while (FLASH->SR & OTA_SR_BSY);
            *((volatile uint16_t*)(flash_addr + i)) = hw;
            while (FLASH->SR & OTA_SR_BSY);
        }
        FLASH->CR = 0;

        // ── Write second 512 bytes ────────────────────────────────────────
        if (sec1 < sector_count) {
            tram_sd_read_sector(sectors[sec1], sdhc, buf);
        } else {
            for (uint16_t i = 0; i < 512; i++) buf[i] = 0xFF;
        }

        FLASH->CR = OTA_CR_PG;
        for (uint16_t i = 0; i < 512; i += 2) {
            uint16_t hw = (uint16_t)buf[i] | ((uint16_t)buf[i + 1] << 8);
            while (FLASH->SR & OTA_SR_BSY);
            *((volatile uint16_t*)(flash_addr + 512 + i)) = hw;
            while (FLASH->SR & OTA_SR_BSY);
        }
        FLASH->CR = 0;
    }

    FLASH->CR |= OTA_CR_LOCK;

    NVIC_SystemReset();
    while (1);  // unreachable
}

// ─── File-scope SD objects ───────────────────────────────────────────────────
//
// We use our own Sd2Card / SdVolume / SdFile instances so we can access the
// low-level FAT internals (firstCluster, blocksPerCluster, etc.) that the
// high-level SD.h File class does not expose.
//
// SdVolume::sdCard_ is a static shared pointer.  Calling s_vol.init() sets it
// to &s_card.  Since s_card talks to the same physical device as the
// DatabaseOnSD library, all subsequent SdFile operations work correctly.

static Sd2Card  s_card;
static SdVolume s_vol;
static SdFile   s_root;
static SdFile   s_file;

// ─── OTAManager ──────────────────────────────────────────────────────────────

OTAManager::OTAManager()
    : _ready(false), _fw_size(0), _sector_count(0), _sdhc(0)
{}

OTAManager* OTAManager::get_instance()
{
    static OTAManager instance;
    return &instance;
}

// ── detect ───────────────────────────────────────────────────────────────────

bool OTAManager::detect(uint32_t* out_size, uint32_t* out_crc32)
{
    if (!SD.exists(OTA_FW_FILENAME)) {
        return false;
    }

    File meta = SD.open(OTA_META_FILENAME);
    if (!meta) {
        DEBUG_INFO_LN("[OTA] firmware.jsn not found");
        return false;
    }

    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, meta);
    meta.close();

    if (err) {
        DEBUG_INFO("[OTA] firmware.jsn parse error: ");
        DEBUG_INFO_LN(err.c_str());
        return false;
    }

    uint32_t size  = doc["size"].as<uint32_t>();
    uint32_t crc32 = doc["crc32"].as<uint32_t>();

    if (size == 0 || size > (128UL * 1024UL)) {
        DEBUG_INFO_LN("[OTA] firmware size out of range");
        return false;
    }

    *out_size  = size;
    *out_crc32 = crc32;

    DEBUG_INFO("[OTA] Detected: ");
    DEBUG_INFO(size);
    DEBUG_INFO(" bytes, crc32=0x");
    DEBUG_INFO_FMT(crc32, HEX);
    DEBUG_INFO_LN("");
    return true;
}

// ── CRC32 verification ───────────────────────────────────────────────────────
//
// Opens firmware.bin via SdFile::read() and computes IEEE 802.3 / zlib CRC32.
// Returns the computed CRC; caller compares against expected.

uint32_t OTAManager::compute_crc32_from_sd(uint32_t fw_size)
{
    const uint32_t poly = 0xEDB88320UL;
    uint32_t crc = 0xFFFFFFFFUL;

    s_file.rewind();

    uint8_t buf[512];
    uint32_t remaining = fw_size;

    while (remaining > 0) {
        uint16_t to_read = (remaining >= 512) ? 512 : (uint16_t)remaining;
        int16_t  got     = s_file.read(buf, to_read);
        if (got != (int16_t)to_read) break;

        for (uint16_t i = 0; i < to_read; i++) {
            crc ^= buf[i];
            for (int b = 0; b < 8; b++)
                crc = (crc & 1) ? ((crc >> 1) ^ poly) : (crc >> 1);
        }
        remaining -= to_read;
    }
    return crc ^ 0xFFFFFFFFUL;
}

// ── FAT sector collection ────────────────────────────────────────────────────
//
// Walks the FAT chain for s_file and records the absolute SD block number of
// every 512-byte sector up to ceil(fw_size / 512).
//
// Uses public SdVolume accessors (blocksPerCluster, dataStartBlock,
// fatStartBlock, fatType, sdCard) plus a manual Sd2Card::readBlock() to read
// each FAT sector — fatGet() is private so we read the FAT raw.

bool OTAManager::collect_sectors(uint32_t fw_size)
{
    uint8_t  bpc        = s_vol.blocksPerCluster();
    uint32_t data_start = s_vol.dataStartBlock();
    uint32_t fat_start  = s_vol.fatStartBlock();
    uint8_t  fat_type   = s_vol.fatType();
    uint32_t cluster    = s_file.firstCluster();
    Sd2Card* card       = SdVolume::sdCard();

    uint16_t needed = (uint16_t)((fw_size + 511) / 512);
    _sector_count = 0;

    static uint8_t fat_buf[512];   // static — keeps off stack for large cluster chains

    while (cluster >= 2 && _sector_count < needed) {

        // Record each block in this cluster (up to the number we still need)
        uint32_t blk = data_start + ((uint32_t)(cluster - 2)) * bpc;
        for (uint8_t b = 0; b < bpc && _sector_count < needed; b++) {
            if (_sector_count >= OTA_MAX_SECTORS) {
                DEBUG_INFO_LN("[OTA] sector list overflow");
                return false;
            }
            _sectors[_sector_count++] = blk + b;
        }

        // Read the FAT entry for this cluster to get the next cluster number
        uint32_t fat_off = (fat_type == 32) ? (cluster * 4UL) : (cluster * 2UL);
        uint32_t fat_blk = fat_start + fat_off / 512;
        uint16_t fat_idx = (uint16_t)(fat_off % 512);

        if (!card->readBlock(fat_blk, fat_buf)) {
            DEBUG_INFO_LN("[OTA] FAT read error");
            return false;
        }

        uint32_t next;
        if (fat_type == 32) {
            next = ((uint32_t)fat_buf[fat_idx]
                 | ((uint32_t)fat_buf[fat_idx + 1] << 8)
                 | ((uint32_t)fat_buf[fat_idx + 2] << 16)
                 | ((uint32_t)fat_buf[fat_idx + 3] << 24)) & 0x0FFFFFFFUL;
            if (next >= 0x0FFFFFF8UL) break;    // end of chain
        } else {
            next = (uint32_t)fat_buf[fat_idx]
                 | ((uint32_t)fat_buf[fat_idx + 1] << 8);
            if (next >= 0xFFF8UL) break;        // end of chain
        }
        cluster = next;
    }

    return (_sector_count > 0);
}

// ── begin ────────────────────────────────────────────────────────────────────

bool OTAManager::begin(uint32_t firmware_size, uint32_t expected_crc32)
{
    _ready        = false;
    _sector_count = 0;
    _fw_size      = firmware_size;

    // ── Init our own low-level SD stack ─────────────────────────────────────
    if (!s_card.init(SPI_HALF_SPEED, SYSTEM_SD_CS_PIN)) {
        DEBUG_INFO_LN("[OTA] SD card init failed");
        return false;
    }
    if (!s_vol.init(&s_card)) {
        DEBUG_INFO_LN("[OTA] FAT volume init failed");
        return false;
    }
    if (!s_root.openRoot(&s_vol)) {
        DEBUG_INFO_LN("[OTA] Root dir open failed");
        return false;
    }
    if (!s_file.open(&s_root, OTA_FW_FILENAME, O_READ)) {
        DEBUG_INFO_LN("[OTA] firmware.bin open failed");
        s_root.close();
        return false;
    }

    _sdhc = (s_card.type() == SD_CARD_TYPE_SDHC);

    // ── CRC32 verification (single sequential pass) ──────────────────────────
    DEBUG_INFO_LN("[OTA] Verifying CRC32...");
    uint32_t actual_crc = compute_crc32_from_sd(firmware_size);

    if (actual_crc != expected_crc32) {
        DEBUG_INFO("[OTA] CRC FAIL: got 0x");
        DEBUG_INFO_FMT(actual_crc, HEX);
        DEBUG_INFO(" expected 0x");
        DEBUG_INFO_FMT(expected_crc32, HEX);
        DEBUG_INFO_LN("");
        s_file.close();
        s_root.close();
        return false;
    }
    DEBUG_INFO_LN("[OTA] CRC OK");

    // ── FAT chain walk: collect sector numbers ───────────────────────────────
    DEBUG_INFO_LN("[OTA] Mapping sectors...");
    if (!collect_sectors(firmware_size)) {
        DEBUG_INFO_LN("[OTA] Sector mapping failed");
        s_file.close();
        s_root.close();
        return false;
    }

    DEBUG_INFO("[OTA] Mapped ");
    DEBUG_INFO(_sector_count);
    DEBUG_INFO_LN(" sectors");

    s_file.close();
    s_root.close();

    _ready = true;
    return true;
}

// ── abort ────────────────────────────────────────────────────────────────────

void OTAManager::abort()
{
    if (s_file.isOpen()) s_file.close();
    if (s_root.isOpen()) s_root.close();
    _ready        = false;
    _sector_count = 0;
    DEBUG_INFO_LN("[OTA] Aborted");
}

// ── apply ────────────────────────────────────────────────────────────────────

void OTAManager::apply()
{
    // Remove the firmware files so a failed boot doesn't re-trigger the update
    SD.remove(OTA_FW_FILENAME);
    SD.remove(OTA_META_FILENAME);

    DEBUG_INFO_LN("[OTA] Starting apply — device will restart in ~4 seconds");
    delay(100);

    // _sectors[] is a member of OTAManager (in .bss / SRAM).
    // ota_ram_apply runs from SRAM and receives a pointer into SRAM — safe.
    ota_ram_apply(_sectors, _sector_count, _fw_size, _sdhc);

    // NEVER REACHED
}
