#pragma once
#include "common/common.h"
#include <PubSubClient.h>
#include "comms_net_client.h"

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

typedef enum {
    MQTT_OK = 0,
    MQTT_ERROR_OUT_OF_MEMORY = -1,
    MQTT_ERROR_NO_CLIENT = -2,
    MQTT_ERROR_UNSUBSCRIBE_FAIL = -3,
    MQTT_ERROR_NOT_CONNECTED = -4,
    MQTT_ERROR_PUBLISH_FAIL = -5
}mqtt_error_t;

class CommsMQTTClient{
    public:
        CommsMQTTClient(Client * network_client);
        ~CommsMQTTClient(void);

        bool connect(void);
        void disconnect(void);
        bool is_connected(void);
        bool publish_event(const char* topic, const char* payload, uint8_t qos = 0);
        static void message_callback(char* topic, byte* payload, unsigned int len);
        static CommsMQTTClient * get_instance(void);
        void loop();
    private:
        mqtt_error_t unsubscribe();
        void _free_client(void);
        Client * _network_client = nullptr;
        PubSubClient * _client = nullptr;
        mqtt_error_t _error;
};


