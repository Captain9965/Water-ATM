#pragma once
#include "common/common.h"
#include <Countdown.h>
#include <MQTTClient.h>
#include <MQTTPacket.h>
#include "comms_ip.h"

/**
 * @brief Setup Topics
 * 
 *  Notes on Topics: 
 * 
 *  birth topic - This is a topic that the device publishes to
 *  when it makes the first MQTT Connection or any other subsequent
 *  reconnections. It is not enforced but encouraged as a good practice.
 * 
 *  will topic - This is a topic that the device should publish to when 
 *  it disconnects. This can be used to detect your device going offline.
 *  It is sent together with the CONNECT payload.
 *  Also not enforced but considered a good practice.
 */

#define MQTT_USERNAME                   "lenny"
#define MQTT_PASSWORD                   "lenny254"
#define MQTT_DEVICE_ID                  "any"
#define MQTT_PUB_TOPIC_PREFIX           "w/p/"
#define MQTT_SUB_TOPIC_PREFIX           "w/s/"
#define MQTT_BIRTH_TOPIC                MQTT_PUB_TOPIC_PREFIX "birth"
#define MQTT_WILL_TOPIC                 MQTT_PUB_TOPIC_PREFIX "will"
#define MQTT_BIRTH_MESSAGE              "CONNECTED"
#define MQTT_WILL_MESSAGE               "DISCONNECTED";
#define MQTT_SERVER                     "broker.hivemq.com"
#define MQTT_PORT                       1883
#define MQTT_MAX_PACKET_SIZE            256
#define MQTT_MAX_MESSAGE_HANDLERS       3
#define MQTT_KEEP_ALIVE_INTERVAL        60

class CommsMQTTClient{
    public:
        CommsMQTTClient(IPStack * ipstack);
        ~CommsMQTTClient(void);

        bool yield(void);
        bool connect(void);
        void disconnect(void);
        bool is_connected(void);
        bool publish_event(const char* topic, const char* payload, MQTT::QoS qos = MQTT::QOS0);
        static void message_callback(MQTT::MessageData &md);
        static CommsMQTTClient * get_instance(void);
    private:
        int unsubscribe();
        void _free_client(void);
        IPStack * _ipstack = nullptr;
        MQTT::Client<IPStack, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_MESSAGE_HANDLERS> * _client = nullptr;
        int8_t _error;
};


