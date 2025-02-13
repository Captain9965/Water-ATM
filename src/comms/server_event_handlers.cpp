#include "server_event_handlers.h"
#include "ArduinoJson.h"
#include "vmc/vmc_data.h"

//event strings
#define PAY_EVENT_STR "pay"

//event handlers
static void handle_pay_event(JsonDocument* doc){
    const char* amount = (*doc)["a"];
    DEBUG_INFO("Amount: ");
    DEBUG_INFO(amount);
    //convert amount to uint32_t
    uint32_t amount_int = atoi(amount);
    //set AdminCash to amount
    AdminCash::get_default_instance()->set(amount_int);
}


void handle_server_side_event(char* event, size_t len)
{

    
    //user ArduinoJson to parse the event, event is structued as a json string, with event type denoted as "ev"
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, event);
    if (error) {
        DEBUG_INFO_LN("Failed to parse event");
        return;
    }
    //unpack json document, and switch on event type
    const char* ev = doc["ev"];
    if(strcmp(ev, PAY_EVENT_STR) == 0){
        handle_pay_event(&doc);
    }
}