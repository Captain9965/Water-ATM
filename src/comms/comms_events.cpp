#include "comms_events.h"
#include "ArduinoJson.h"
QueueHandle_t * CommsOutQueue::get_instance(){
    static QueueHandle_t out_queue = xQueueCreate(COMMS_EVENTS_QUEUE_SIZE, sizeof(struct outMessage));
    return &out_queue;
}


QueueHandle_t * CommsInQueue::get_instance(){
    static QueueHandle_t in_queue = xQueueCreate(COMMS_EVENTS_QUEUE_SIZE, sizeof(struct inMessage));
    return &in_queue;
}

comms_ev_error_t publish_check_event(check_event_t * event){
    outMessage_t msg;
    
    if(create_check_event_payload(msg.message, sizeof(msg.message))!= COMMS_EV_OK){
        return COMMS_EV_PAYLOAD_ERROR;
    }
    DEBUG_INFO("Enqueue event-> ");
    DEBUG_INFO_LN(msg.message);
    if(!xQueueSend(* CommsOutQueue::get_instance(), (void *)&msg, 0)){
        DEBUG_INFO_LN("Queue is full!");
        return COMMS_EV_QUEUE_FULL;
    }
    return COMMS_EV_OK;

}

static comms_ev_error_t create_check_event_payload(char * buffer, ssize_t len){
    std::string out;
    StaticJsonDocument<256> doc;
    int rss = 0;
    std::string version = "0.1.1";

    doc["ev"] = "check";
    doc["ms"] = millis();
    doc["ver"] = version;
    doc["uid"] = stm32f1_uid();

    serializeJson(doc, out);
    const char* temp_buffer = out.c_str();
    memset(buffer, 0, len);
    memcpy(buffer, temp_buffer, strlen(temp_buffer));
    return COMMS_EV_OK;
}

