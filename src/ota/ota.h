#pragma once
#include "common/common.h"

// ─── SD-card direct-write OTA for STM32F103RB (128 KB flash) ────────────────
//
// No staging area in flash — the SD card IS the staging buffer.
// The full 128 KB of flash is available to the application.
//
// ── Update trigger ──────────────────────────────────────────────────────────
// Place two files in the SD card root before powering on:
//
//   firmware.bin   – raw binary produced by the build (no header)
//   firmware.jsn  – metadata: {"size":<uint32>,"crc32":<uint32>}
//                    crc32 is IEEE 802.3 / zlib over the whole binary
//
// On boot the device:
//   1. Detects both files and parses metadata.
//   2. Reads the entire firmware.bin once to verify CRC32.
//   3. Walks the FAT chain to record every 512-byte sector number.
//   4. Enters vmc_ota, deletes the SD files, then calls ota_ram_apply().
//
// ── RAM trampoline ───────────────────────────────────────────────────────────
// ota_ram_apply() is in the .RamFunc section (copied to SRAM at startup).
// Running from SRAM it can safely erase all 128 pages of application flash,
// reading replacement data sector-by-sector from the SD card using raw SPI
// register access (the SD library is no longer available once flash is gone).
//
// ── Power-loss risk ──────────────────────────────────────────────────────────
// CRC is verified BEFORE any flash is touched.  The vulnerable window is the
// ~3-4 seconds ota_ram_apply() runs.  A power loss in that window leaves flash
// in a partially-erased state → device requires ST-Link reflash.

#define OTA_APP_ADDR      0x08000000UL
#define OTA_PAGE_SIZE     1024UL           // STM32F103 medium-density: 1 KB pages
#define OTA_MAX_SECTORS   256U             // ceil(128 KB / 512) — sector list in SRAM

#define OTA_FW_FILENAME   "firmware.bin"
#define OTA_META_FILENAME "firmware.jsn"

class OTAManager {
public:
    static OTAManager* get_instance();

    // Scan SD root for firmware.bin + firmware.jsn and parse metadata.
    // Returns true when a valid update package is present.
    bool detect(uint32_t* out_size, uint32_t* out_crc32);

    // Verify the firmware CRC32 and build the sector map from the FAT chain.
    // On success is_ready() returns true and apply() may be called.
    // Returns false if the file is missing, CRC fails, or FAT walk fails.
    bool begin(uint32_t firmware_size, uint32_t expected_crc32);

    // Apply the update.
    // Deletes SD files, then calls ota_ram_apply() which erases application
    // flash page-by-page, reads each sector from SD via raw SPI, writes to
    // flash, and finally calls NVIC_SystemReset().  NEVER RETURNS.
    void apply();

    // Cancel a pending session (e.g. stale state detected at boot).
    void abort();

    bool     is_ready()  const { return _ready; }
    uint32_t get_fw_size()const { return _fw_size; }

private:
    OTAManager();

    bool     _ready;
    uint32_t _fw_size;
    uint32_t _sectors[OTA_MAX_SECTORS]; // absolute SD block numbers for firmware.bin
    uint16_t _sector_count;
    uint8_t  _sdhc;                     // 1 = SDHC (block addr), 0 = SDSC (byte addr)

    uint32_t compute_crc32_from_sd(uint32_t fw_size);
    bool     collect_sectors(uint32_t fw_size);
};
