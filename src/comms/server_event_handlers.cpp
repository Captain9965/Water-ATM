#include "server_event_handlers.h"
#include "ArduinoJson.h"
#include "vmc/vmc_data.h"

//event strings
#define PAY_EVENT_STR "pay"
#define CARD_CONFIG_EVENT_STR "card_config"

//event handlers
static void handle_pay_event(JsonDocument* doc){
    const char* amount = (*doc)["a"];
    std::string uid = (*doc)["u"];

    DEBUG_INFO("Amount: ");
    DEBUG_INFO(amount);
    //convert amount to uint32_t
    uint32_t amount_int = atoi(amount);
    //set AdminCash to amount
    AdminCash::get_default_instance()->set(amount_int);

    // send prec event to server to acknowledge receipt of funds:
    prec_event_t prec_event;

    prec_event.uid = uid;
    publish_prec_event(&prec_event);
}

/*handle admin card and service card config event*/
static void handle_card_config_event(JsonDocument* doc){
    
    String admin_card_uid = (*doc)["admin"];
    String service_card_uid = (*doc)["service"];
    DEBUG_INFO("Admin card: ");
    DEBUG_INFO(admin_card_uid);
    DEBUG_INFO("Service card: ");
    DEBUG_INFO(service_card_uid);

    //set admin card and service card
    AdminCard::get_default_instance()->set(admin_card_uid);
    ServiceCard::get_default_instance()->set(service_card_uid);
    return;   
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
    } else if(strcmp(ev, CARD_CONFIG_EVENT_STR) == 0){
        handle_card_config_event(&doc);
    }
}