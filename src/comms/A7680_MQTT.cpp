#include "A7680_MQTT.h"

A7680_MQTT::A7680_MQTT(Stream& modem, Stream& debug) : 
  _modem(&modem), _debug(&debug), _rssi(99), _currentState(INIT), _callback(nullptr) {}

void A7680_MQTT::begin(uint8_t power_key) {
  _powerKey = power_key;
  _apnIndex = 0;
  _apn      = APN_LIST[0].apn;
  pinMode(_powerKey, OUTPUT);
  digitalWrite(_powerKey, LOW);
}

void A7680_MQTT::powerOn() {
  _debug->println("Triggering Modem Power Key...");
  digitalWrite(_powerKey, HIGH);
  delay(2000);
  digitalWrite(_powerKey, LOW);
  _apnIndex = 0;                      // restart APN search from the top
  _apn      = APN_LIST[0].apn;
  _debug->println("Waiting for modem boot (approx 10s)...");
  // We don't block for 15s here; we let the state machine poll 'AT'
}

void A7680_MQTT::setCallback(MQTTCallback cb) {
  _callback = cb;
}

bool A7680_MQTT::advanceAPN() {
  _apnIndex++;
  if (_apnIndex >= APN_LIST_SIZE) {
    _apnIndex = 0;
    _apn      = APN_LIST[0].apn;
    _debug->println("[APN] All candidates exhausted — restarting search.");
    return false;
  }
  _apn = APN_LIST[_apnIndex].apn;
  _debug->print("[APN] Trying next (");
  _debug->print(_apnIndex);
  _debug->print("/");
  _debug->print(APN_LIST_SIZE - 1);
  _debug->print("): ");
  _debug->println(_apn);
  return true;
}

bool A7680_MQTT::sendAT(const char* cmd, const char* expected, uint32_t timeout) {
  // _debug->print(">> "); _debug->println(cmd);
  _modem->println(cmd);
  uint32_t t = millis();
  String resp = "";
  while (millis() - t < timeout) {
    while (_modem->available()) {
      char c = _modem->read();
      resp += c;
      if (resp.indexOf(expected) != -1) {
        // Save bytes that aren't the expected response back to _rxBuf.
        // A concurrent URC (e.g. incoming MQTT chunk) may have arrived in
        // the UART stream between the command echo and the expected string.
        // Without this, those bytes are silently discarded and the URC is
        // lost, leaving _rxBuf with only a partial / stale URC fragment.
        int matchStart = resp.indexOf(expected);
        int matchEnd   = matchStart + (int)strlen(expected);
        String surplus = resp.substring(0, matchStart) + resp.substring(matchEnd);
        if (surplus.length() > 0) {
          _rxBuf += surplus;
          _rxLastByteMs = millis();
        }
        return true;
      }
      if (resp.indexOf("ERROR") != -1) {
        // Save any bytes before ERROR — they may contain a URC.
        int errPos = resp.indexOf("ERROR");
        if (errPos > 0) {
          _rxBuf += resp.substring(0, errPos);
          _rxLastByteMs = millis();
        }
        return false;
      }
    }
  }
  // Timeout — save everything accumulated; it's likely an unsolicited URC.
  if (resp.length() > 0) {
    _rxBuf += resp;
    _rxLastByteMs = millis();
  }
  return false;
}

void A7680_MQTT::update() {
  // Drain all available bytes into _rxBuf and note the timestamp of the last byte.
  while (_modem->available()) {
    _rxBuf += (char)_modem->read();
    _rxLastByteMs = millis();
  }

  // Process the buffer once the bus has been idle for RX_IDLE_MS ms.
  // This handles URCs that arrive in multiple bursts without the 1-second
  // blocking wait that readString() imposed.
  if (_rxBuf.length() > 0 && (millis() - _rxLastByteMs) >= RX_IDLE_MS) {
    if (isMQTTURCComplete(_rxBuf)) {
      _rxBuf.trim();
      // _debug->print("RAW MODEM: "); _debug->println(_rxBuf);
      processIncoming(_rxBuf);
      _rxBuf = "";
    } else if ((millis() - _rxLastByteMs) >= 2000) {
      // Stale incomplete URC (payload never arrived) — discard to prevent lockup.
      _debug->println("RX: stale incomplete URC, discarding.");
      _rxBuf = "";
    }
    // else: MQTT URC is still arriving across multiple FIFO refills — keep accumulating.
  }

  switch (_currentState) {
    case INIT:
      if (sendAT("AT", "OK", 500)) {
        // Rate-limit SIM checks — modem needs time to detect a reinserted SIM.
        if (millis() - _lastSimCheck < _simRetryInterval) break;
        _lastSimCheck = millis();
        _debug->println("Modem Responsive. Checking SIM...");
        if (sendAT("AT+CPIN?", "READY")) {
            _simRetryCount = 0;
            _currentState = NET_OPEN;
            // Get IMEI for Client ID
            getIMEI();
            _debug->print("Modem IMEI: "); _debug->println(_imei);
            _clientID = _imei; // Use IMEI as MQTT Client ID
            _topic = String(MQTT_SUB_TOPIC_PREFIX) + _imei; // Subscribe to unique topic based on IMEI
        } else {
            _simRetryCount++;
            _debug->print("SIM not ready — retry ");
            _debug->print(_simRetryCount);
            _debug->print("/");
            _debug->println(SIM_MAX_RETRIES);
            if (_simRetryCount >= SIM_MAX_RETRIES) {
                _debug->println("SIM unresponsive — cycling modem power.");
                _simRetryCount = 0;
                powerOn();
            }
        }
      } else {
        // If not responsive, try to power on
        static uint32_t lastPowerRetry = 0;
        if (millis() - lastPowerRetry > 15000) {
            powerOn();
            lastPowerRetry = millis();
        }
      }
      break;

    case NET_OPEN:

      // 1. RSSI Check for Debugging/Stability
      sendAT("AT+CSQ"); // parseCSQ will update _rssi
      _debug->print("Current RSSI: "); _debug->println(_rssi);
      
      if (_rssi == 99 || _rssi < 5) {
          _debug->println("Signal too weak or searching...check antenna...");
          delay(2000);

      }

      // 2. Check if already open (prevents redundant AT+NETOPEN calls which cause ERROR)
      if (sendAT("AT+NETOPEN?", "+NETOPEN: 1")) {
          _debug->println("Network already open.");
          _currentState = MQTT_START;
          break;
      } else {
          _debug->println("Network not open. Attempting to open...");
      }

      // 3. Ensure we are physically registered on the tower
      if (!sendAT("AT+CREG?", "+CREG: 0,1") && !sendAT("AT+CREG?", "+CREG: 0,5")) {
          _debug->println("Tower registration missing. Retrying...");
          return; // Stay in this state
      } else {
          _debug->println("Tower registration confirmed.");
      }

      // 4. Ensure GPRS is attached (this is the data component of the signal)
      if (!sendAT("AT+CGATT?", "+CGATT: 1")) {
          _debug->println("Attaching GPRS...");
          sendAT("AT+CGATT=1", "OK", 5000);
          return; 
      } else {
          _debug->println("GPRS attached.");
      }

      // 5. Set the APN
      if (sendAT(("AT+CGDCONT=1,\"IP\",\"" + _apn + "\"").c_str())) {
          _debug->println("APN set successfully.");
      } else {
          _debug->println("Failed to set APN.");
      }

      // 6. Open the Network
      _debug->print("Attempting NETOPEN with APN: ");
      _debug->println(_apn);
      if (sendAT("AT+NETOPEN", "OK", 5000)) {
          // Wait for the unsolicited result +NETOPEN: 0 (Success)
          // Some firmware versions return OK immediately but take time to finalize
          delay(1000);
          if (sendAT("AT+NETOPEN?", "+NETOPEN: 1")) {
              _debug->print("Network opened successfully. APN: ");
              _debug->println(_apn);
              _currentState = MQTT_START;
          } else {
              _debug->println("NETOPEN accepted but network not open — trying next APN.");
              if (!advanceAPN()) _currentState = ERROR_RECOVERY;
          }
      } else {
          _debug->println("NETOPEN failed — trying next APN.");
          if (!advanceAPN()) _currentState = ERROR_RECOVERY;
      }
      break;

    case MQTT_START: {
      _debug->println("Ensuring MQTT service is stopped...");
      sendAT("AT+CMQTTSTOP", "OK", 2000);
      delay(1000);

      if (sendAT("AT+CMQTTSTART", "OK", 3000)) {
        _debug->println("MQTT Service Started.");
        delay(1000);  // MQTT service needs time to fully initialize before ACCQ
      } else {
        _debug->println("MQTT Start failed/already running — proceeding.");
        delay(500);
      }

      // Release any lingering client before re-acquiring
      sendAT("AT+CMQTTREL=0", "OK", 1000);
      delay(500);

      // Acquire client — retry once on failure (modem may still be initializing)
      String accqCmd = "AT+CMQTTACCQ=0,\"" + _clientID + "\",0";
      if (!sendAT(accqCmd.c_str(), "OK", 3000)) {
          _debug->println("ACCQ failed, retrying in 1s...");
          delay(1000);
          if (!sendAT(accqCmd.c_str(), "OK", 3000)) {
              _debug->println("Could not acquire MQTT Client — recovering.");
              _currentState = ERROR_RECOVERY;
              break;
          }
      }
      _debug->println("MQTT Client ID acquired successfully.");
      _currentState = MQTT_CONN;
      break;
    }
    case MQTT_CONN:

      _debug->println("Connecting to Broker: 52.146.3.202...");
      // Wait for +CMQTTCONNECT: 0,0 — the async CONNACK confirmation.
      // The modem sends "OK" when the command is accepted, then sends
      // "+CMQTTCONNECT: 0,0" once the broker's CONNACK is received.
      // Proceeding on "OK" alone causes the next command (subscribe) to fail
      // because the modem is still mid-handshake.
      if (sendAT("AT+CMQTTCONNECT=0,\"tcp://52.146.3.202:1883\",60,0", "+CMQTTCONNECT: 0,0", 30000)) {
          _debug->println("MQTT Connected Successfully!");
          _currentState = MQTT_SUBSCRIBE;
      } else {
          _debug->println("MQTT Connection rejected or timed out.");
          _currentState = ERROR_RECOVERY;
      }
      break;

    case MQTT_SUBSCRIBE:
    
    subscribe(_topic);
    
    break;
    case MQTT_IDLE:
      if (millis() - _lastHeartbeat > _heartbeatInterval) {
        publish("heartbeat", "alive");
        _lastHeartbeat = millis();
      }
      if (millis() - _lastRSSICheck > _rssiInterval) {
        sendAT("AT+CSQ");
        _lastRSSICheck = millis();
      }
      // Independently verify the network layer is still up.
      // +CMQTTCONNLOST can be missed if the URC arrives mid-AT-exchange;
      // this poll catches silent disconnects regardless of URC delivery.
      if (millis() - _lastNetCheck > _netCheckInterval) {
        _lastNetCheck = millis();
        if (!sendAT("AT+NETOPEN?", "+NETOPEN: 1", 2000)) {
          _debug->println("Network check failed — triggering recovery.");
          _currentState = ERROR_RECOVERY;
        }
      }
      break;

    case ERROR_RECOVERY:
      _debug->println("Recovery Mode: Resetting MQTT/Network...");
      sendAT("AT+CMQTTDISC=0,60");
      sendAT("AT+CMQTTSTOP");
      sendAT("AT+NETCLOSE");
      delay(2000);
      _modem->flush();
      _currentState = INIT;
      break;
  }
}

bool A7680_MQTT::isMQTTURCComplete(const String& buf) {
  // If no MQTT receive URC in buffer, it's an AT response — process normally.
  int topicIdx = buf.indexOf("+CMQTTRXTOPIC:");
  if (topicIdx == -1) return true;

  // We have a topic header; wait until the payload header also arrives.
  int payloadIdx = buf.indexOf("+CMQTTRXPAYLOAD:");
  if (payloadIdx == -1) return false;

  // Parse the declared payload byte count: "+CMQTTRXPAYLOAD: 0,<len>"
  int comma   = buf.indexOf(',', payloadIdx);
  int lineEnd = buf.indexOf('\n', payloadIdx);
  if (comma == -1 || lineEnd == -1 || comma > lineEnd) return false;

  int expectedLen = buf.substring(comma + 1, lineEnd).toInt();
  // Payload data begins on the line immediately following the header.
  int dataStart = lineEnd + 1;
  return ((int)buf.length() - dataStart) >= expectedLen;
}

void A7680_MQTT::processIncoming(String rx) {
  // _debug->print("<< "); _debug->println(rx);
  
  // Check for the RSSI update first
  if (rx.indexOf("+CSQ:") != -1) {
    parseCSQ(rx);
    return;
  }

  // Handle the segmented MQTT response
  if (rx.indexOf("+CMQTTRXTOPIC:") != -1 && rx.indexOf("+CMQTTRXPAYLOAD:") != -1) {
    // _debug->println("!!! PARSING SEGMENTED MQTT DATA !!!");

    // 1. Extract Topic
    // Look for the line immediately following +CMQTTRXTOPIC: 0,19
    int topicIdx = rx.indexOf("+CMQTTRXTOPIC:");
    int topicLineStart = rx.indexOf('\n', topicIdx) + 1;
    int topicLineEnd = rx.indexOf('\r', topicLineStart);
    String topic = rx.substring(topicLineStart, topicLineEnd);
    topic.trim();

    // 2. Extract Payload
    // Look for the line immediately following +CMQTTRXPAYLOAD: 0,64
    int payloadIdx = rx.indexOf("+CMQTTRXPAYLOAD:");
    int payloadLineStart = rx.indexOf('\n', payloadIdx) + 1;
    int payloadLineEnd = rx.indexOf('\r', payloadLineStart);
    String payload = rx.substring(payloadLineStart, payloadLineEnd);
    payload.trim();

    _debug->println("Parsed Topic: " + topic);
    _debug->println("Parsed Payload: " + payload);

    if (_callback != nullptr) {
      _callback(topic, payload);
    }
  } 
  else if (rx.indexOf("+CMQTTCONNLOST") != -1 || rx.indexOf("CLOSED") != -1) {
    _debug->println("Connection lost detected in URC.");
    _currentState = ERROR_RECOVERY;
  }
}

void A7680_MQTT::parseRX(String rx) {
  int first = rx.indexOf(',');
  int second = rx.indexOf(',', first + 1);
  int third = rx.indexOf(',', second + 1);
  int fourth = rx.indexOf(',', third + 1);
  if (fourth != -1 && _callback != nullptr) {
    String topic = rx.substring(second + 1, third);
    String payload = rx.substring(fourth + 1);
    payload.trim();
    _callback(topic, payload);
  }
}

void A7680_MQTT::parseCSQ(String rx) {
  int colon = rx.indexOf(':');
  int comma = rx.indexOf(',');
  if (colon != -1 && comma != -1) {
    _rssi = rx.substring(colon + 1, comma).toInt();
  }
}

bool  A7680_MQTT::publish(const char* topic, const char* msg) {
  // debug topic and message:
    // _debug->print("Publishing to topic: ");
    // _debug->print(topic);
    // _debug->print(" | Message: ");
    // _debug->println(msg);
  char cmd[64];
  sprintf(cmd, "AT+CMQTTTOPIC=0,%d", (int)strlen(topic));
  if (sendAT(cmd, ">")) {
    _modem->print(topic);
    delay(30);
  } else {
    _debug->println("Failed to set topic — connection lost, recovering.");
    _currentState = ERROR_RECOVERY;
    return false;
  }
  sprintf(cmd, "AT+CMQTTPAYLOAD=0,%d", (int)strlen(msg));
  if (sendAT(cmd, ">")) {
    _modem->print(msg);
    delay(30);
  } else {
    _debug->println("Failed to set payload — connection lost, recovering.");
    _currentState = ERROR_RECOVERY;
    return false;
  }
  // QoS 0 (fire-and-forget): modem sends +CMQTTPUB: 0,0 as soon as the TCP
  // packet is queued, without waiting for a broker PUBACK.  This avoids
  // blocking the next publish while the broker round-trip completes.
  // Wait for +CMQTTPUB: 0,0 to ensure the modem has fully committed the
  // message before we return — otherwise the next AT+CMQTTTOPIC is rejected.
  if (sendAT("AT+CMQTTPUB=0,0,60", "+CMQTTPUB: 0,0", 5000)) {
    _debug->println("Publish successful.");
    return true;
  } else {
    _debug->println("Publish failed.");
    _currentState = ERROR_RECOVERY;
    return false;
  }
}

void A7680_MQTT::subscribe(String topic) {

  String cmd = "AT+CMQTTSUBTOPIC=0," + String(topic.length()) + ",0";
  _debug->print("Subscribing to topic: "); _debug->println(topic);
    if (sendAT(cmd.c_str(), ">")) {
          _modem->print(topic);
          delay(100); // Give it a heartbeat to digest
          
          // 2. Execute the subscription.
          // Wait for +CMQTTSUB: — the async SUBACK confirmation from the broker.
          if (sendAT("AT+CMQTTSUB=0", "+CMQTTSUB:", 10000)) {
              _debug->println("Subscribed successfully.");
              _lastHeartbeat = millis();
              _currentState = MQTT_IDLE;
          } else {
              _currentState = ERROR_RECOVERY;
          }
      } else {
          _debug->println("Failed to set subscription topic.");
          _currentState = ERROR_RECOVERY;
      }
}

int A7680_MQTT::getRSSI() { return _rssi; }
bool A7680_MQTT::isConnected() { return _currentState == MQTT_IDLE; }

String A7680_MQTT::getIMEI() {
    if (_imei != "") return _imei; // Return cached value if already read

    _modem->println("AT+GSN");
    uint32_t t = millis();
    String resp = "";
    
    while (millis() - t < 5000) {
        while (_modem->available()) {
            char c = _modem->read();
            if (isDigit(c)) resp += c; // IMEI is only digits
        }
        if (resp.length() >= 15) { // Standard IMEI length
            _imei = resp;
            return _imei;
        }
    }
    return ""; // Return empty string if failed to read
}