#include "mqtt_callback.h"

void mqtt_server_event_callback(String topic, String payload){
    /* handle server side event at this point */
    if (topic != String(MQTT_SUB_TOPIC_PREFIX) + String(A7680_MQTT::get_default_instance()->getIMEI().c_str())){
        DEBUG_INFO("Received message on unknown topic: ");
        DEBUG_INFO_LN(topic.c_str());
        return;
    }

    char display_buffer[payload.length() + 1];
    snprintf(display_buffer, sizeof(display_buffer),"%s",payload.c_str());
    DEBUG_INFO("Received Message -> ");
    DEBUG_INFO_LN(display_buffer);
    handle_server_side_event(display_buffer, payload.length());
}