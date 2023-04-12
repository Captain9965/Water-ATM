
#include "main.h"
#include "stm32f1_uid.h"

CommsIpstack * _comms_ip = nullptr;
CommsMQTTClient * mqttClient = nullptr;
void setup() {
  // put your setup code here, to run once:
    int error = 0;
    DEBUG_INIT(&error);
    if (error != 0){
      DEBUG_LN("Serial init failed!!");
    }

    // GSM ON
    _comms_ip = CommsGsmStack::get_instance();
    _comms_ip->init();
    mqttClient = CommsMQTTClient::get_instance();
   
}

void loop() {
  // put your main code here, to run repeatedly:
    DEBUG_LN("Starting up");
    while(1){
      DEBUG("Is network connected(0 False / 1 True)? : ");
      DEBUG_LN(_comms_ip->connected());
      if (!_comms_ip->connected() || !mqttClient->is_connected())
      {
          if (!_comms_ip->connect())
          {
              DEBUG_LN("[ERROR] GPRS Reconnection failed. Trying again.");
          }

          // Let's reconnect to the broker

          // Clean up connection???
          if (mqttClient){
              mqttClient->disconnect();
          }
          
          if (!mqttClient->connect())
          {
              DEBUG_LN("[ERROR] Failed to reconnnect to the broker. Trying again.");
          }
      }
      if (mqttClient){
        mqttClient->yield();
      }
      
      const char * topic = (std::string(MQTT_PUB_TOPIC_PREFIX) + stm32f1_uid()).c_str();
      /* send dummy message: */
      mqttClient->publish_event(topic, "Hello");

      delay(1500); 
    }
}

