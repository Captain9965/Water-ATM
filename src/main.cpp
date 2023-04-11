#define TINY_GSM_MODEM_SIM800
#include <Arduino.h>
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>
#include <MQTTPacket.h>
#include <TinyGsmClient.h>
#include "main.h"


const char apn[] = "iot.safaricom.com";
const char user[] = "none";
const char pass[] = "none";

HardwareSerial GSMSerial(PA3, PA2);

#define SerialMon Serial
#define SerialAT GSMSerial

#define GSM_POWER_KEY PB12
//#define DUMP_AT_COMMANDS

#define TINY_GSM_DEBUG SerialMon

// Uses an extra library
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient tinyGSMClient(modem);


const char mqttUsername[] = MQTT_CREDENTIALS;
const char mqttPassword[] = DEVICE_GROUP_PASSWORD;
const char mqttDeviceID[] = DEVICE_ID;


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
const char *birthTopic = TOPIC_PREFIX "birth";
const char *willTopic = TOPIC_PREFIX "will";
const char * topic = TOPIC_PREFIX "669ff52-48487183-67222131";

const char birthMessage[] = "CONNECTED";
const char willMessage[] = "DISCONNECTED";

char brokerAddress[] = "broker.hivemq.com";
int brokerPort = 1883;

// END MQTT CONFIG


bool brokerConnect(void);
bool gsmConnect(void);
void getModemData(void);
void incomingMessageHandler(MQTT::MessageData &messageData);
void publishMessage(char *payload, const char *topic);

constexpr unsigned int str2int(const char *str, int h)
{
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

IPStack * ipstack = nullptr;
MQTT::Client<IPStack, Countdown, 128, 3> * mqttClient = nullptr;

char buffer[100];
int returnCode = 0;

int modemConnAttemptsCount = 0;

void setup() {
  // put your setup code here, to run once:
    SerialMon.setRx(PA10);
    SerialMon.setTx(PA9);
    SerialMon.begin(115200);
    delay(100);
    SerialAT.begin(115200);
    delay(100);

    // GSM ON
    pinMode(GSM_POWER_KEY, OUTPUT);
    digitalWrite(GSM_POWER_KEY, 1);
    delay(3000);
    digitalWrite(GSM_POWER_KEY, 0);

    delay(100);
    while (!SerialMon || !SerialAT)
    {
        ; // Serial Not working
    }

    
}

void loop() {
  // put your main code here, to run repeatedly:
    SerialMon.println("Starting up");
    getModemData();
    while(1){
      sprintf(buffer, "Is network connected(0 False / 1 True)? : %i ", modem.isNetworkConnected());
      SerialMon.println(buffer);
      if (!modem.isGprsConnected() || !mqttClient || !mqttClient->isConnected())
      {
          if (gsmConnect()== false)
          {
              SerialMon.println("[ERROR] GPRS Reconnection failed. Trying again.");
          }

          // Let's reconnect to the broker

          // Clean up connection???
          if (mqttClient){
              mqttClient->disconnect();
          }
          
          if (!brokerConnect())
          {
              SerialMon.println("[ERROR] Failed to reconnnect to the broker. Trying again.");
          }
      }
      if (mqttClient){
        mqttClient->yield();
      }
      

      /* send dummy message: */
      publishMessage("Hello","w/p/669ff52-48487183-67222131");

      delay(1500); 
    }
}

void getModemData(void)
{
    String name = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(name);
    String info = modem.getModemInfo();
    SerialMon.print("Modem Name: ");
    SerialMon.println(info);
    String ccid = modem.getSimCCID();
    SerialMon.print("CCID: ");
    SerialMon.println(ccid);
    String imei = modem.getIMEI();
    SerialMon.print("IMEI: ");
    SerialMon.println(imei);
    String imsi = modem.getIMSI();
    SerialMon.print("IMSI: ");
    SerialMon.println(imsi);
    String cop = modem.getOperator();
    SerialMon.print("Operator: ");
    SerialMon.println(cop);
}

bool gsmConnect(void)
{
    if (modemConnAttemptsCount > 0)
    {
        SerialMon.println("Modem reconnnection had been attempted earlier. Restarting");
        // FIXME: We're doing this in another piece of C code, but I fully do not understand the implications of regularly switching on the power-key
        digitalWrite(GSM_POWER_KEY, 1);
        delay(3000); // Should we do this here?????
        digitalWrite(GSM_POWER_KEY, 0);
        modem.restart();
    }
    sprintf(buffer, "Getting the modem ready \r\n");
    SerialMon.print(buffer);
    modem.init();
    sprintf(buffer, "Initializing GSM network registration...\r\n");
    SerialMon.print(buffer);
    if (!modem.waitForNetwork())
    {
        sprintf(buffer, "\r\n Unable to initialize registration. Reset and try again.\r\n");
        SerialMon.print(buffer);
        modemConnAttemptsCount++;
        return false; // Exit
    }
    sprintf(buffer, "GSM OK\r\n");
    SerialMon.print(buffer);

    sprintf(buffer, "Attempting to establish GPRS connection \r\n");
    SerialMon.print(buffer);
    if (!modem.gprsConnect(apn, user, pass))
    {
        sprintf(buffer, "Unable to connect to APN. Reset and try again \r\n");
        SerialMon.print(buffer);
        modemConnAttemptsCount++;
        return false; // Exit
    }

    sprintf(buffer, "GSM is Okay \r\n");
    SerialMon.print(buffer);
    modemConnAttemptsCount = 0;
    return true;
}


bool brokerConnect(void)
{
    MQTT::Message mqttMessage;
    snprintf(buffer, sizeof(buffer), "Connecting to %s on port %i \r\n", brokerAddress, brokerPort);
    if (ipstack){
      delete ipstack;
      ipstack = nullptr;
    }
    ipstack =  new IPStack(tinyGSMClient);
    if (!ipstack){
      return false;
    }
    returnCode = ipstack->connect(brokerAddress, brokerPort);
    SerialMon.println(buffer);
    if (returnCode != 1)
    {
        snprintf(buffer, sizeof(buffer), "Unable to connect to Broker TCP Port. \r\n");
        SerialMon.println(buffer);
        return false; // Exit immediately
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "Broker TCP port open \r\n");
        SerialMon.println(buffer);
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *)mqttDeviceID;
    data.username.cstring = (char *)mqttUsername;
    data.password.cstring = (char *)mqttPassword;
    data.keepAliveInterval = 60;
    data.cleansession = 1;
    data.will.message.cstring = (char *)willMessage;
    data.will.qos = MQTT::QOS1;
    data.will.retained = 0;
    data.will.topicName.cstring = (char *)willTopic;

    if(mqttClient){
      delete mqttClient;
      mqttClient = nullptr;
    }
    mqttClient = new MQTT::Client<IPStack, Countdown, 128, 3>(*ipstack);
    if(!mqttClient){
      return false;
    }
    returnCode = mqttClient->connect(data);
    if (returnCode != 0)
    {
        snprintf(buffer, sizeof(buffer), "Error establishing connection session with  broker. Error Code %i. \r\n", returnCode);
        SerialMon.print(buffer);
        return false; // Exit immediately
    }
    mqttMessage.qos = MQTT::QOS1;
    mqttMessage.retained = false;
    mqttMessage.dup = false;
    mqttMessage.payload = (void *)birthMessage;
    mqttMessage.payloadlen = strlen(birthMessage) + 1;
    returnCode = mqttClient->publish(birthTopic, mqttMessage);

    snprintf(buffer, sizeof(buffer), "Birth topic publish return code %i \n", returnCode);
    SerialMon.println(buffer);

    returnCode = mqttClient->subscribe(topic, MQTT::QOS1, incomingMessageHandler);
    if (returnCode != 0)
    {
        snprintf(buffer, sizeof(buffer), "Unable to subscribe to servo topic. Hanging the process\r\n");
        SerialMon.print(buffer);
        return false; // Exit immediately
    }
    /* subscribe to other topics here:
     maybe I need to add provision to connect 
     to other topics as arguments to this function*/
    snprintf(buffer, sizeof(buffer), "Successfully connected to the broker\n");
    SerialMon.println(buffer);
    return true;
}


void publishMessage(char *payload, const char *topic)
{
    MQTT::Message message;
    message.qos = MQTT::QOS1;
    message.payload = (void *)payload;
    message.retained = 0;
    message.payloadlen = strlen(payload) + 1;
    returnCode = mqttClient->publish(topic, message);
    snprintf(buffer, sizeof(buffer), "%s topic publish return code %i \n", topic, returnCode);
    SerialMon.println(buffer);
}

void incomingMessageHandler(MQTT::MessageData &messageData)
{
    char cmd[messageData.message.payloadlen + 1];
    MQTT::Message &message = messageData.message;
    snprintf(cmd, sizeof(cmd), "%s", messageData.message.payload);
    SerialMon.print(F("Incoming message: "));
    SerialMon.println(cmd);
    memset((char *)message.payload, NULL, sizeof(cmd));
}

