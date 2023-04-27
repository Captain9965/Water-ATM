#include "mqtt.h"

CommsMQTTClient::CommsMQTTClient(Client * network_client):_network_client(network_client){

}

CommsMQTTClient::~CommsMQTTClient(void){
    _free_client();
}

void CommsMQTTClient::_free_client(void){
    if (_client != nullptr){
        delete _client;
        _client = nullptr;
    }
    _network_client->stop();
}

bool CommsMQTTClient::connect(void){
    DEBUG_INFO_LN("CommsMQTTClient::connect(void)");

    /* maybe configure topic using the uid of the chip??*/
    std::string uid = stm32f1_uid();
    std::string topic = std::string(MQTT_SUB_TOPIC_PREFIX) + uid;
    DEBUG_INFO("MCU uid -> ");
    DEBUG_INFO_LN(uid.c_str());

    /* incase this is a reconnection */
    _free_client();

    DEBUG_INFO_LN("Creating client..");

    _client = new PubSubClient(MQTT_SERVER, MQTT_PORT, &CommsMQTTClient::message_callback , * _network_client);

    if (!_client){
        DEBUG_INFO_LN("Cannot instantiate client!!");
        /**
         * @todo: create more custom errors!!
         * 
         */
        _error = MQTT_ERROR_OUT_OF_MEMORY;
        return false;
    }

    DEBUG_INFO_LN("Connecting to MQTT broker");
    if (!_client->connect(MQTT_DEVICE_ID, MQTT_USERNAME, MQTT_PASSWORD)){
        DEBUG_INFO_LN("Error connecting to MQTT broker -> ");
        _free_client();
        return false;
    }

    DEBUG_INFO("Subscribing to topic: ");
    DEBUG_INFO_LN(topic.c_str());


    if (!_client->subscribe(topic.c_str(), 1)){
        DEBUG_INFO("Unable to subscribe to topic: ");
        _free_client();
        return false;
    }
    return true;
}

void CommsMQTTClient::disconnect(void){
    DEBUG_INFO_LN("Unsubscribing and Disconnecting mqtt client");
    _error = unsubscribe();
    if (_client){
        _client->disconnect();
    }
    _free_client();
}

 mqtt_error_t CommsMQTTClient::unsubscribe(){
    DEBUG_INFO_LN("Unsubscribing from topic");
    const char * topic = (std::string(MQTT_SUB_TOPIC_PREFIX) + stm32f1_uid()).c_str();
    if (!_client){
        return MQTT_ERROR_NO_CLIENT;
    }
    if (!_client->unsubscribe(topic)){
        DEBUG_INFO("Failed to unsubscribe to topic : ");
        _error = MQTT_ERROR_UNSUBSCRIBE_FAIL;
        DEBUG_INFO_LN(topic);
        return MQTT_ERROR_UNSUBSCRIBE_FAIL;
    }
    return MQTT_OK;
}

void CommsMQTTClient::message_callback(char* topic, byte* payload, unsigned int len){
    std::string uid = stm32f1_uid();
    std::string sub_topic = std::string(MQTT_SUB_TOPIC_PREFIX) + uid;
    if (String(topic) == sub_topic.c_str()){
        DEBUG_INFO("Received Message -> ");
        /* handle server side event at this point */
        char display_buffer[len + 1];
        snprintf(display_buffer, sizeof(display_buffer),"%s",payload);
        DEBUG_INFO_LN(display_buffer);
    }
    
    return;
}

void CommsMQTTClient::loop(){
    if(_client){
         _client->loop();
    }
}


bool CommsMQTTClient::is_connected(void){
    if(!_network_client || !_client || !_client->connected()){
        return false;
    }
    return true;
}

bool CommsMQTTClient::publish_event(const char* topic, const char* payload, uint8_t qos){
    if (!is_connected()){
        _error = MQTT_ERROR_NOT_CONNECTED;
        DEBUG_INFO("Publish Event: Client not connected");
        DEBUG_INFO_LN(_error);
        return false;
    }
    DEBUG_INFO("MQTT send event -> ");
    DEBUG_INFO_LN(payload);


    if (!_client->publish(topic, payload)){
        
        DEBUG_INFO("Publish fail");
        _error = MQTT_ERROR_PUBLISH_FAIL;
        return false;
    }
    return true;
}

CommsMQTTClient * CommsMQTTClient::get_instance(void){
    static CommsMQTTClient client = CommsMQTTClient(GsmClient::get_instance()->get_client());
    return &client;
}
