#include "mqtt_ota.h"
#include "ota/ota.h"            // OTA_FW_FILENAME, OTA_META_FILENAME
#include "comms/comms_events.h" // MQTT_PUB_TOPIC_PREFIX
#include "comms/A7680_MQTT.h"
#include "vmc/vmc_flags.h"
#include "vmc/vmc.h"

MqttOtaHandler* MqttOtaHandler::get_instance() {
    static MqttOtaHandler inst;
    return &inst;
}

MqttOtaHandler::MqttOtaHandler()
    : _active(false), _total_chunks(0), _next_idx(0),
      _fw_size(0), _fw_crc(0), _last_ms(0) {}

// ── ota_start ─────────────────────────────────────────────────────────────────
// {"ev":"ota_start","size":<bytes>,"chunks":<N>,"crc32":<uint32>}
void MqttOtaHandler::on_ota_start(JsonDocument* doc) {
    uint32_t chunks = (*doc)["chunks"] | (uint32_t)0;
    uint32_t size   = (*doc)["size"]   | (uint32_t)0;
    uint32_t crc32  = (*doc)["crc32"]  | (uint32_t)0;

    // Reject while dispensing — server will retry after BUSY_RETRY_S seconds.
    const char* state = VMC::get_default_instance()->get_state()->id;
    if (state != nullptr && strcmp(state, "DISPENSING") == 0) {
        DEBUG_INFO_LN("[MQTT OTA] Busy — dispensing in progress");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"busy\"}");
        return;
    }

    if (chunks == 0 || size == 0) {
        DEBUG_INFO_LN("[MQTT OTA] Bad ota_start metadata");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"bad_start\"}");
        return;
    }

    reset();  // discard any partial previous session

    _total_chunks = chunks;
    _fw_size      = size;
    _fw_crc       = crc32;

    SD.remove(OTA_FW_FILENAME);
    _file = SD.open(OTA_FW_FILENAME, FILE_WRITE);
    if (!_file) {
        DEBUG_INFO_LN("[MQTT OTA] Cannot open firmware.bin");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"sd_open\"}");
        return;
    }

    _active  = true;
    _last_ms = millis();
    set_vmc_flag(VMC_OTA_ACTIVE);

    // Advertise chunk size and starting index (fresh start → next=0).
    char buf[56];
    snprintf(buf, sizeof(buf), "{\"ev\":\"ota_ready\",\"cs\":%u,\"next\":0}",
             MQTT_OTA_CHUNK_SIZE);
    publish_response(buf);

    DEBUG_INFO("[MQTT OTA] Ready — ");
    DEBUG_INFO(chunks);
    DEBUG_INFO_LN(" chunks");
}

// ── ota_chunk ─────────────────────────────────────────────────────────────────
// {"ev":"ota_chunk","idx":<n>,"data":"<hex>"}
void MqttOtaHandler::on_ota_chunk(JsonDocument* doc) {
    if (!_active) {
        DEBUG_INFO_LN("[MQTT OTA] ota_chunk with no active session");
        return;
    }

    // Stale session
    if ((millis() - _last_ms) > MQTT_OTA_TIMEOUT_MS) {
        DEBUG_INFO_LN("[MQTT OTA] Session timed out");
        reset();
        return;
    }

    // Abort if dispensing started after ota_start was accepted.
    const char* state = VMC::get_default_instance()->get_state()->id;
    if (state != nullptr && strcmp(state, "DISPENSING") == 0) {
        DEBUG_INFO_LN("[MQTT OTA] Aborting — dispensing started mid-transfer");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"busy\"}");
        reset();
        return;
    }

    uint32_t    idx  = (*doc)["idx"]  | (uint32_t)0;
    const char* data = (*doc)["data"];

    if (!data) {
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"no_data\"}");
        return;
    }

    // Duplicate chunk — our previous ota_ack was lost in transit.
    // Reply seq_err so the server advances without resetting the session.
    if (idx < _next_idx) {
        DEBUG_INFO("[MQTT OTA] Duplicate idx=");
        DEBUG_INFO_LN(idx);
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"seq_err\"}");
        return;
    }

    // Truly out-of-order — fatal for this session
    if (idx != _next_idx) {
        DEBUG_INFO("[MQTT OTA] Out-of-order: expected ");
        DEBUG_INFO(_next_idx);
        DEBUG_INFO(" got ");
        DEBUG_INFO_LN(idx);
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"seq_err\"}");
        reset();
        return;
    }

    // Decode and write
    uint8_t buf[MQTT_OTA_CHUNK_SIZE];
    size_t  len = 0;

    if (!hex_decode(data, buf, &len)) {
        DEBUG_INFO_LN("[MQTT OTA] Hex decode failed");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"decode\"}");
        reset();
        return;
    }

    if (_file.write(buf, len) != (int)len) {
        DEBUG_INFO_LN("[MQTT OTA] SD write error");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"sd_write\"}");
        reset();
        return;
    }

    _next_idx++;
    _last_ms = millis();

    // ACK
    char ack[36];
    snprintf(ack, sizeof(ack), "{\"ev\":\"ota_ack\",\"idx\":%lu}", (unsigned long)idx);
    publish_response(ack);

    DEBUG_INFO("[MQTT OTA] ");
    DEBUG_INFO(_next_idx);
    DEBUG_INFO("/");
    DEBUG_INFO_LN(_total_chunks);

    if (_next_idx >= _total_chunks) {
        finalize();
    }
}

// ── tick ──────────────────────────────────────────────────────────────────────
// Called every loop from otaPage::update().  If the server drops off mid-
// transfer no more ota_chunk messages arrive and the in-message timeout check
// never fires.  This guarantees the session is torn down after MQTT_OTA_TIMEOUT_MS
// regardless of whether any messages are coming in.
void MqttOtaHandler::tick() {
    if (!_active) return;
    if ((millis() - _last_ms) > MQTT_OTA_TIMEOUT_MS) {
        DEBUG_INFO_LN("[MQTT OTA] Session timed out — server dropped off");
        reset();   // clears _active + VMC_OTA_ACTIVE → ota_page returns to idle
    }
}

// ── ota_abort ─────────────────────────────────────────────────────────────────
void MqttOtaHandler::on_ota_abort() {
    DEBUG_INFO_LN("[MQTT OTA] Abort received");
    reset();
}

// ── finalize ──────────────────────────────────────────────────────────────────
// All chunks received.  Write firmware.jsn, send ota_done, reboot.
void MqttOtaHandler::finalize() {
    _file.close();

    SD.remove(OTA_META_FILENAME);
    File meta = SD.open(OTA_META_FILENAME, FILE_WRITE);
    if (!meta) {
        DEBUG_INFO_LN("[MQTT OTA] Cannot write firmware.jsn");
        publish_response("{\"ev\":\"ota_err\",\"msg\":\"sd_meta\"}");
        reset();
        return;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "{\"size\":%lu,\"crc32\":%lu}",
             (unsigned long)_fw_size, (unsigned long)_fw_crc);
    meta.print(buf);
    meta.close();

    // Notify server before reset so it knows the transfer succeeded.
    publish_response("{\"ev\":\"ota_done\"}");

    _active = false;
    DEBUG_INFO_LN("[MQTT OTA] Transfer complete — rebooting to apply update");
    delay(500);   // give the modem time to deliver ota_done
    NVIC_SystemReset();
}

// ── reset ─────────────────────────────────────────────────────────────────────
void MqttOtaHandler::reset() {
    if (_file) _file.close();
    SD.remove(OTA_FW_FILENAME);
    clear_vmc_flag(VMC_OTA_ACTIVE);
    _active       = false;
    _total_chunks = 0;
    _next_idx     = 0;
    _fw_size      = 0;
    _fw_crc       = 0;
    _last_ms      = 0;
}

// ── publish_response ──────────────────────────────────────────────────────────
void MqttOtaHandler::publish_response(const char* json_str) {
    String imei  = A7680_MQTT::get_default_instance()->getIMEI();
    String topic = String(MQTT_PUB_TOPIC_PREFIX) + imei;
    A7680_MQTT::get_default_instance()->publish(topic.c_str(), json_str);
}

// ── hex_decode ────────────────────────────────────────────────────────────────
bool MqttOtaHandler::hex_decode(const char* hex, uint8_t* out, size_t* out_len) {
    if (!hex) return false;
    size_t hex_len = strlen(hex);
    if (hex_len == 0 || (hex_len % 2) != 0) return false;

    *out_len = hex_len / 2;
    if (*out_len > MQTT_OTA_CHUNK_SIZE) return false;

    for (size_t i = 0; i < *out_len; i++) {
        auto nibble = [](uint8_t c) -> int8_t {
            if (c >= '0' && c <= '9') return (int8_t)(c - '0');
            if (c >= 'a' && c <= 'f') return (int8_t)(c - 'a' + 10);
            if (c >= 'A' && c <= 'F') return (int8_t)(c - 'A' + 10);
            return -1;
        };
        int8_t h = nibble((uint8_t)hex[2 * i]);
        int8_t l = nibble((uint8_t)hex[2 * i + 1]);
        if (h < 0 || l < 0) return false;
        out[i] = ((uint8_t)h << 4) | (uint8_t)l;
    }
    return true;
}
