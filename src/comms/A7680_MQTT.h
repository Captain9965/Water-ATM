#pragma once

#include "common/common.h"
#include "server_event_handlers.h"
#include "comms/apn_list.h"

// function signature for the callback
typedef void (*MQTTCallback)(String topic, String payload);

class A7680_MQTT {
  public:
    A7680_MQTT(Stream& modem, Stream& debug);
    void begin(uint8_t power_key);
    void setCallback(MQTTCallback cb);
    void update();
    bool publish(const char* topic, const char* msg);
    void subscribe(String topic);
    int getRSSI();
    bool isConnected();
    String getIMEI();
    void powerOn();
    
    static A7680_MQTT* get_default_instance() {
        static A7680_MQTT instance(MODEM, SerialDebug);
        return &instance;
    }

  private:
    enum State { INIT, NET_OPEN, MQTT_START, MQTT_CONN, MQTT_SUBSCRIBE, MQTT_IDLE, ERROR_RECOVERY };
    
    Stream* _modem;
    Stream* _debug;
    String _apn;
    String _clientID;
    int _rssi;
    State _currentState;
    MQTTCallback _callback;
    String _imei = "";
    String _topic = "";
    uint8_t _powerKey;
    
    uint8_t  _apnIndex = 0;

    uint32_t _lastHeartbeat;
    uint32_t _lastRSSICheck;
    uint32_t _lastNetCheck  = 0;
    uint32_t _lastSimCheck  = 0;
    uint8_t  _simRetryCount = 0;
    static constexpr uint8_t SIM_MAX_RETRIES = 3;
    const uint32_t _heartbeatInterval = 50000;
    const uint32_t _rssiInterval      = 60000;
    const uint32_t _netCheckInterval  = 30000; // poll AT+NETOPEN? every 30 s
    const uint32_t _simRetryInterval  = 5000;  // retry AT+CPIN? every 5 s

    // Accumulation buffer for incoming UART bytes.
    // Processed once the bus has been idle for RX_IDLE_MS milliseconds,
    // which reliably marks the end of a complete URC burst.
    String   _rxBuf;
    uint32_t _rxLastByteMs = 0;
    // Wait 100 ms of bus silence before processing a URC.
    // The A7680 delivers a 256-byte chunk URC (~640 bytes) by refilling its
    // internal UART TX buffer 2-3 times.  Each refill can introduce a brief
    // gap; 100 ms covers any realistic inter-burst pause while still being
    // 10× faster than the original 1000 ms readString() timeout.
    static constexpr uint32_t RX_IDLE_MS = 100;

    // Advances _apnIndex to the next candidate and updates _apn.
    // Returns false (and wraps back to 0) when the list is exhausted.
    bool advanceAPN();

    bool sendAT(const char* cmd, const char* expected = "OK", uint32_t timeout = 3000);
    void parseCSQ(String rx);
    void parseRX(String rx);
    void processIncoming(String rx);
    // Returns true when _rxBuf holds a complete receivable unit.
    // For MQTT receive URCs, waits until both the topic header AND the full
    // payload bytes have arrived (length declared in +CMQTTRXPAYLOAD: 0,<len>).
    bool isMQTTURCComplete(const String& buf);
};