#pragma once
#include <Arduino.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <MQTTPacket.h>

#define TINY_GSM_MODEM_SIM800
// #define DUMP_AT_COMMANDS
#define SYSTEM_DEBUG
#define ERROR_DEBUG
#define WARNING_DEBUG
#define INFO_DEBUG
#define APN_NAME                    "iot.safaricom.com"
#define CELLULAR_USER_NAME          "none"
#define CELLULAR_PASSWORD           "none"
#define SYSTEM_GSM_BAUDRATE         115200
#define SYSTEM_DEBUG_BAUDRATE       115200

#define SerialDebug Serial

void debug_init(int * error);
#ifdef SYSTEM_DEBUG
#define DEBUG_INIT(x) debug_init(x)
#define DEBUG(x) SerialDebug.print(x)
#define DEBUG_LN(x) SerialDebug.println(x)
#else
#define DEBUG(x)
#define DEBUG_INIT(x)
#define DEBUG_LN(x)
#endif
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

#define SYSTEM_GSM_TX_PIN                   PA2
#define SYSTEM_GSM_RX_PIN                   PA3
#define SYSTEM_GSM_POWER_KEY                PB12
#define SYSTEM_SERIAL_DEBUG_RX              PA10
#define SYSTEM_SERIAL_DEBUG_TX              PA9


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
#define MQTT_MAX_PACKET_SIZE            100
#define MQTT_MAX_MESSAGE_HANDLERS       3
#define MQTT_KEEP_ALIVE_INTERVAL        60

#include <IPStack.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
#endif

class CommsIpstack{
    public:
        CommsIpstack(){};
        ~CommsIpstack(){};
        virtual void init() = 0;
        virtual bool connect() = 0;
        virtual bool connected() = 0;
        virtual int16_t get_signal_strength() = 0;
        virtual IPStack * get_ipstack() = 0;
    
};

class CommsGsmStack: public CommsIpstack{
    public:
        CommsGsmStack(){};
        ~CommsGsmStack(){};
        void init()override;
        void modem_power_on();
        String get_modem_imei();
        int16_t get_signal_strength()override;
        String get_network_operator();
        bool is_network_connected();
        bool is_GPRS_connected();
        bool connect()override;
        bool connected()override;
        bool connect_to_network();
        bool connect_to_gprs();
        void increment_reconnection_count();
        void reset_reconnection_count();
        static CommsIpstack * get_instance();
        IPStack * get_ipstack()override;
    private:
        /* declare modem serial interface */
        HardwareSerial GSMSerial = HardwareSerial(SYSTEM_GSM_RX_PIN, SYSTEM_GSM_TX_PIN);

        #ifdef DUMP_AT_COMMANDS
        StreamDebugger debugger = StreamDebugger(GSMSerial, SerialDebug);
        TinyGsm modem = TinyGsm(debugger);
        #else
        TinyGsm modem = TinyGsm(GSMSerial);
        #endif
        
        TinyGsmClient tinyGSMClient = TinyGsmClient(modem);
        IPStack ipstack = IPStack(tinyGSMClient);
        int8_t modem_connection_attempt_count = 0;
};

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
        void _free_client(void);
        IPStack * _ipstack = nullptr;
        MQTT::Client<IPStack, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_MESSAGE_HANDLERS> * _client = nullptr;
        int8_t _error;
};
