#include "comms.h"
#include "vmc/vmc_flags.h"

Comms::Comms() {}

Comms* Comms::get_instance(){
    static Comms comms;
    return &comms;
}

void Comms::init(){
    ModemSerial.begin(SYSTEM_GSM_BAUDRATE);
    A7680_MQTT* mqtt = A7680_MQTT::get_default_instance();
    mqtt->begin(SYSTEM_GSM_POWER_KEY);
    mqtt->setCallback(mqtt_server_event_callback);
    mqtt->powerOn();
}

void Comms::update(){
    A7680_MQTT* mqtt = A7680_MQTT::get_default_instance();
    mqtt->update();

    if (mqtt->isConnected()){
        static bool _imei_printed = false;
        if (!_imei_printed){
            DEBUG_INFO("MQTT connected. IMEI: ");
            DEBUG_INFO_LN(mqtt->getIMEI().c_str());
            _imei_printed = true;
        }
        set_vmc_flag(VMC_NET_CONNECTED);
    } else {
        clear_vmc_flag(VMC_NET_CONNECTED);
    }

    dispatch_comms_queue();
}

int16_t Comms::get_rss(){
    return (int16_t)A7680_MQTT::get_default_instance()->getRSSI();
}

comms_ev_error_t Comms::dispatch_comms_queue(){
    if (!A7680_MQTT::get_default_instance()->isConnected()){
        return COMMS_EV_QUEUE_EMPTY;
    }
    outMessage_t msg;
    if(!CommsOutQueue::get_instance()->peek(msg)){
        return COMMS_EV_QUEUE_EMPTY;
    }
    DEBUG_INFO("Dispatching event-> ");
    DEBUG_INFO_LN(msg.message);
    String imei = A7680_MQTT::get_default_instance()->getIMEI();
    std::string topic = std::string(MQTT_PUB_TOPIC_PREFIX) + imei.c_str();
    if(A7680_MQTT::get_default_instance()->publish(topic.c_str(), msg.message)){
        CommsOutQueue::get_instance()->dequeue(msg);
        return COMMS_EV_OK;
    }
    return COMMS_EV_PAYLOAD_ERROR;
}
