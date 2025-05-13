#include "server_event_handlers.h"
#include "ArduinoJson.h"
#include "vmc/vmc_data.h"
#include "vmc/vmc.h"

//event strings
#define PAY_EVENT_STR "pay"
#define CARD_CONFIG_EVENT_STR "card_config"
#define CALIBRATON_CONFIG_EVENT_STR "calibration_config"
#define TARIFF_CONFIG_EVENT_STR "tariff_config"
#define TIME_CONFIG_EVENT_STR "time_config"

//event handlers
static void handle_pay_event(JsonDocument* doc){
    const char* amount = (*doc)["a"];
    std::string uid = (*doc)["u"];

    DEBUG_INFO("Amount: ");
    DEBUG_INFO(amount);
    //convert amount to uint32_t
    uint32_t amount_int = atoi(amount);
    //check state id to see whether we are in dispensing state
    if(VMC::get_default_instance()->get_state()->id == "DISPENSING"){
        //if we are in dispensing state, we need to acknowledge receipt of funds and set admin cash
        AdminCash::get_default_instance()->set(amount_int);
        prec_event_t prec_event;
        prec_event.uid = uid;
        publish_prec_event(&prec_event);
        return;
    }
    //debug
    DEBUG_INFO("Not in dispensing state, ignoring pay event");
    return;
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

/*handle calibration config event*/
static void handle_calib_config_event(JsonDocument* doc){
    float calib1 = (*doc)["calib1"];
    float calib2 = (*doc)["calib2"];
    float calib3 = (*doc)["calib3"];
    float calib4 = (*doc)["calib4"];
    float flowCalibration = (*doc)["flow_calibration"];

    DEBUG_INFO("Calibration 1: ");
    DEBUG_INFO(calib1);
    DEBUG_INFO("Calibration 2: ");
    DEBUG_INFO(calib2);
    DEBUG_INFO("Calibration 3: ");
    DEBUG_INFO(calib3);
    DEBUG_INFO("Calibration 4: ");
    DEBUG_INFO(calib4);
    DEBUG_INFO("Flow_calibration: ");
    DEBUG_INFO(flowCalibration);

    calibration_t calib;
    calib.calibration1 = calib1;
    calib.calibration2 = calib2;
    calib.calibration3 = calib3;
    calib.calibration4 = calib4;
    /*set calibration: */
    Calibration::get_default_instance()->set(calib);

    /*set flow calibration: */
    FlowCalculationTime::get_default_instance()->set(flowCalibration);
    return;
}

/*handle tariff config event*/
static void handle_tariff_config_event(JsonDocument* doc){
    float tariff1 = (*doc)["tariff1"];
    float tariff2 = (*doc)["tariff2"];
    float tariff3 = (*doc)["tariff3"];
    float tariff4 = (*doc)["tariff4"];

    DEBUG_INFO("Tariff 1: ");
    DEBUG_INFO(tariff1);
    DEBUG_INFO("Tariff 2: ");
    DEBUG_INFO(tariff2);
    DEBUG_INFO("Tariff 3: ");
    DEBUG_INFO(tariff3);
    DEBUG_INFO("Tariff 4: ");
    DEBUG_INFO(tariff4);

    tariff_t tariff;
    tariff.tariff1 = tariff1;
    tariff.tariff2 = tariff2;
    tariff.tariff3 = tariff3;
    tariff.tariff4 = tariff4;
    /*set tariff: */
    Tariff::get_default_instance()->set(tariff);
    return;
}

/*handle time config event*/
static void handle_time_config_event(JsonDocument* doc){
    int hour = (*doc)["hour"];
    int minute = (*doc)["minute"];
    int second = (*doc)["second"];
    int day = (*doc)["day"];
    int month = (*doc)["month"];
    int year = (*doc)["year"];
    DEBUG_INFO("Time: ");
    DEBUG_INFO(hour);
    DEBUG_INFO(":");
    DEBUG_INFO(minute);
    DEBUG_INFO(":");
    DEBUG_INFO(second);
    DEBUG_INFO("Date: ");
    DEBUG_INFO(day);
    DEBUG_INFO("/");
    DEBUG_INFO(month);
    DEBUG_INFO("/");
    DEBUG_INFO(year);
    /* set time*/
    system_time_t time;
    time.day = day;
    time.month = month;
    time.year = year;
    time.hour = hour;
    time.minutes = minute;
    time.seconds = second;
    systemTime::get_default_instance()->setTime(time);
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
    } else if(strcmp(ev, CALIBRATON_CONFIG_EVENT_STR) == 0){
        handle_calib_config_event(&doc);
    } else if(strcmp(ev, TARIFF_CONFIG_EVENT_STR) == 0){
        //handle tariff config event
        handle_tariff_config_event(&doc);
    } else if(strcmp(ev, TIME_CONFIG_EVENT_STR) == 0){
        //handle time config event
        handle_time_config_event(&doc);
    }
}