#include "main.h"
#include "stm32f1_uid.h"

CommsMQTTClient::CommsMQTTClient(IPStack * ipstack):_ipstack(ipstack){

}

CommsMQTTClient::~CommsMQTTClient(void){
    _free_client();
}

void CommsMQTTClient::_free_client(void){
    if (_client != nullptr){
        delete _client;
        _client = nullptr;
    }
    _ipstack->disconnect();
}

bool CommsMQTTClient::connect(void){
    DEBUG_LN("CommsMQTTClient::connect(void)");

    /* maybe configure topic using the uid of the chip??*/
    std::string uid = stm32f1_uid();
    std::string topic = std::string(MQTT_SUB_TOPIC_PREFIX) + uid;
    DEBUG("MCU uid -> ");
    DEBUG_LN(uid.c_str());

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *)uid.c_str();
    data.username.cstring = (char *)MQTT_USERNAME;
    data.password.cstring = (char *)MQTT_PASSWORD;
    data.keepAliveInterval = MQTT_KEEP_ALIVE_INTERVAL;
    data.cleansession = 1;
    data.will.message.cstring = (char *)MQTT_WILL_MESSAGE;
    data.will.qos = MQTT::QOS1;
    data.will.retained = 0;
    data.will.topicName.cstring = (char *)MQTT_WILL_TOPIC;

    /* incase this is a reconnection */
    _free_client();

    DEBUG_LN("Opening broker TCP Port");

    if ((_error = _ipstack->connect((char*)MQTT_SERVER, MQTT_PORT)) != 1 ){
        DEBUG_LN("Failed to connect to open broker TCP PORT");
        return false;
    }

    /* we now have our TCP port, now connect to broker*/

    _client = new MQTT::Client<IPStack, Countdown,MQTT_MAX_PACKET_SIZE,MQTT_MAX_MESSAGE_HANDLERS>(* _ipstack);

    if (!_client){
        DEBUG_LN("Cannot instantiate client!!");
        /**
         * @todo: create more custom errors!!
         * 
         */
        _error = -1;
        return false;
    }

    DEBUG_LN("Connecting to MQTT broker");
    if ((_error = _client->connect(data)) != 0){
        DEBUG_LN("Error connecting to MQTT broker -> ");
        DEBUG(_error);
        _free_client();
        return false;
    }

    DEBUG("Subscribing to topic: ");
    DEBUG_LN(topic.c_str());

    if ((_error = _client->subscribe(topic.c_str(), MQTT::QOS0, &CommsMQTTClient::message_callback)) != 0){
        DEBUG("Unable to subscribe to topic: ");
        DEBUG_LN(_error);
        _free_client();
        return false;
    }
    return true;
}

void CommsMQTTClient::disconnect(void){
    DEBUG_LN("Disconnecting mqtt client");
    if (_client){
        _client->disconnect();
    }
    _free_client();
}

void CommsMQTTClient::message_callback(MQTT::MessageData &md){
    MQTT::Message &message = md.message;
    DEBUG("Received Message -> ");
    DEBUG_LN((char *)message.payload);
    /* handle server side event at this point */
    return;
}

bool CommsMQTTClient::yield(void){
    if (!is_connected()){
        _error = -2;
        DEBUG_LN("CommsMQTTClient::yield(void) -> No connection");
        return false;
    }

    if (!_client){
        _error = -3;
        return false;
    }

    /* apparently we get a yield error when we have not received a message within the keepalive window*/
    if ((_error = _client->yield()) != 0){
        // DEBUG("MQTT yield error -> ");
        // DEBUG_LN(_error);
        // _free_client();
        // return false;
    }
    return true;
}

bool CommsMQTTClient::is_connected(void){
    if(!_ipstack || !_client || !_client->isConnected()){
        return false;
    }
    return true;
}

bool CommsMQTTClient::publish_event(const char* topic, const char* payload, MQTT::QoS qos){
    if (!is_connected()){
        _error = -2;
        DEBUG("Publish Event: Client not connected");
        DEBUG_LN(_error);
        return false;
    }
    DEBUG("MQTT send event -> ");
    DEBUG_LN(payload);

    /* Do we add a null terminator??*/
    MQTT::Message message = {qos, false, false, 1, (void *)payload, strlen(payload) + 1};
    if (_error = _client->publish(topic, message) < 0){
        /* I do not yet know the proper return codes: */
        DEBUG("Publish error code : ");
        DEBUG_LN(_error);
        return false;
    }
    return true;
}

CommsMQTTClient * CommsMQTTClient::get_instance(void){
    static CommsMQTTClient client = CommsMQTTClient(CommsGsmStack::get_instance()->get_ipstack());
    return &client;
}
