#include "comms.h"


Comms::Comms(){
    _ipstack = CommsGsmStack::get_instance();
    _mqtt_client = CommsMQTTClient::get_instance();
    return;
}

Comms::~Comms(){
    if(_ipstack){
        delete _ipstack;
        _ipstack = nullptr;
    }
    if(_mqtt_client){
        delete _mqtt_client;
        _mqtt_client = nullptr;
    }

}

Comms* Comms::get_instance(){
    static Comms comms;
    return &comms;
}

void Comms::run(){
    DEBUG_LN("Comms::run()");
    init();
    DEBUG_LN("Comms init success");
    while (1){
        if (!_ipstack->connected() || !_mqtt_client->is_connected()){
                if(!_ipstack->connect()){
                    DEBUG_LN("Failed to connect to network || GPRS");
                }
                _mqtt_client->disconnect();
                if(!_mqtt_client->connect()){
                    DEBUG_LN("Failed to connect to broker");
                }
                last_send_time = millis();
        }
        _mqtt_client->yield();
        DEBUG("Signal strength: ");
        DEBUG_LN(_ipstack->get_signal_strength());

        if(millis() - last_send_time > 20000){
            if(!_mqtt_client->publish_event((std::string(MQTT_PUB_TOPIC_PREFIX) + stm32f1_uid()).c_str(), "{Hello}")){
                DEBUG_LN("Failed to publish message");
            }
            last_send_time = millis();
        }

        delay(1500);
    }

}

/* initialize comms resources here: */
void Comms::init(){
    _ipstack->init();
    return;
}

int16_t Comms::get_rss(){
    return _ipstack->get_signal_strength();
}

