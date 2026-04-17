#pragma once
#include "common/common.h"
#include "SD.h"
#include "ArduinoJson.h"

// Binary bytes per chunk — must match CHUNK_SIZE in ota_flash.py (256).
#define MQTT_OTA_CHUNK_SIZE  256U
// Reset the session if no chunk arrives within this window.
#define MQTT_OTA_TIMEOUT_MS  30000UL

// ─── MqttOtaHandler ──────────────────────────────────────────────────────────
//
// Implements the device side of the ota_flash.py transfer protocol.
//
// Protocol (server → device on topic w/s/<IMEI>):
//
//   {"ev":"ota_start","size":<bytes>,"chunks":<N>,"crc32":<uint32>}
//       → device opens firmware.bin, replies:
//         {"ev":"ota_ready","cs":256,"next":0}
//
//   {"ev":"ota_chunk","idx":<n>,"data":"<hex>"}
//       → device writes chunk, replies:
//         {"ev":"ota_ack","idx":<n>}
//         If duplicate (server retransmit after lost ack):
//         {"ev":"ota_err","msg":"seq_err"}   (session stays alive)
//
//   After the last chunk:
//       device writes firmware.jsn, replies:
//         {"ev":"ota_done"}
//       then calls NVIC_SystemReset().
//       On the next boot vmc_booting detects the two files and the RAM
//       trampoline in ota.cpp flashes the new firmware.
//
//   {"ev":"ota_abort"}
//       → device resets and deletes any partial firmware.bin.

class MqttOtaHandler {
public:
    static MqttOtaHandler* get_instance();

    void on_ota_start(JsonDocument* doc);
    void on_ota_chunk(JsonDocument* doc);
    void on_ota_abort();
    void tick();   // call every loop — enforces the session timeout

    bool    is_active()        const { return _active; }
    uint8_t get_progress_pct() const {
        if (_total_chunks == 0) return 0;
        return (uint8_t)((_next_idx * 100UL) / _total_chunks);
    }

private:
    MqttOtaHandler();

    bool     _active;
    uint32_t _total_chunks;
    uint32_t _next_idx;
    uint32_t _fw_size;
    uint32_t _fw_crc;
    uint32_t _last_ms;
    File     _file;

    void publish_response(const char* json_str);
    void reset();
    void finalize();
    static bool hex_decode(const char* hex, uint8_t* out, size_t* out_len);
};
