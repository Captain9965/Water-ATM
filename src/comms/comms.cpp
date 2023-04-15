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
    DEBUG_INFO_LN("Comms::run()");
    init();
    DEBUG_INFO_LN("Comms init success");
    while (1){
       switch (comms_state){
        case COMMS_STATE_OFF:
            comms_sleep();
            break;
        default:
            comms_loop();
            break;
       };
    if (millis() - toggle_on_time > 60000){
        comms_state == COMMS_STATE_HIGH_FREQUENCY ? set_comms_state(COMMS_STATE_OFF) : init();
        toggle_on_time = millis();
    }
    delay(1500);
    }

}

/* initialize comms resources here and set state: */
void Comms::init(){
    _ipstack->init();
    set_comms_state(COMMS_STATE_HIGH_FREQUENCY);
    return;
}

int16_t Comms::get_rss(){
    return _ipstack->get_signal_strength();
}


void Comms::set_comms_state(comms_state_t state){
    if (comms_state != state){
        comms_state = state;
    }
}
void Comms::comms_loop(){
    if (!_ipstack->connected() || !_mqtt_client->is_connected()){
                if(!_ipstack->connect()){
                    DEBUG_INFO_LN("Failed to connect to network || GPRS");
                }
                _mqtt_client->disconnect();
                if(!_mqtt_client->connect()){
                    DEBUG_INFO_LN("Failed to connect to broker");
                }
                last_send_time = millis();
                COMMS_ON = true;
        }
        _mqtt_client->yield();
        DEBUG_INFO("Signal strength: ");
        DEBUG_INFO_LN(_ipstack->get_signal_strength());


        if(millis() - last_send_time > 20000){
            const char * topic = (std::string(MQTT_PUB_TOPIC_PREFIX) + stm32f1_uid()).c_str();
            if(!_mqtt_client->publish_event(topic, "{Hello}")){
                DEBUG_INFO_LN("Failed to publish message");
            }
            last_send_time = millis();
        }

}

void Comms::comms_sleep(){
    if (COMMS_ON){
        _mqtt_client->disconnect();
        _ipstack->disconnect();
        COMMS_ON = false;
    }
 
    return;
}