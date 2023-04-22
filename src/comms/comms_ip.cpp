#include "comms_ip.h"

/**
 * @todo: 
 * Remove these delays!!!!
 * 
 */
void CommsGsmStack::init(){
    DEBUG_INFO_LN("Powering on modem");
    GSMSerial.begin(SYSTEM_GSM_BAUDRATE);
    wait_ms(100);
    pinMode(SYSTEM_GSM_POWER_KEY, OUTPUT);
    modem_power_on();
}

void CommsGsmStack::modem_power_on(){
    DEBUG_INFO_LN("Turning on modem");
    modem_cycle_power();
    DEBUG_INFO("Found Modem IMEI: ");
    DEBUG_INFO_LN(get_modem_imei());
}
void CommsGsmStack::modem_power_off(){
    DEBUG_INFO_LN("Turning off modem");
    modem.poweroff();
    wait_ms(300);
}

void CommsGsmStack::modem_cycle_power(){
    digitalWrite(SYSTEM_GSM_POWER_KEY, HIGH);
    wait_ms(3000);
    digitalWrite(SYSTEM_GSM_POWER_KEY, LOW);
    wait_ms(100);
}

String CommsGsmStack::get_modem_imei(){
    return modem.getIMEI();
}

int16_t CommsGsmStack::get_signal_strength(){
    return modem.getSignalQuality();
}

String CommsGsmStack::get_network_operator(){
    return modem.getOperator();
}

bool CommsGsmStack::is_GPRS_connected(){
    return modem.isGprsConnected();
}

bool CommsGsmStack::is_network_connected(){
    return modem.isNetworkConnected();
}
bool CommsGsmStack::connected(){
    if(!is_network_connected()){
        DEBUG_INFO_LN("Network is not connected");
        return false;
    }
    if(!is_GPRS_connected()){
        DEBUG_INFO_LN("GPRS is not connected");
        return false;
    }
    return true;
}

bool CommsGsmStack::connect(){
    if (modem_connection_attempt_count > 0){
        DEBUG_INFO("Reconnecting to network, restarting modem Reconnection count is -> ");
        DEBUG_INFO_LN(modem_connection_attempt_count);
        modem_power_off();
        modem_power_on();
        modem.restart();
    }
    modem.init();
    if (!connect_to_network() || !connect_to_gprs()){
        return false;
    }
    reset_reconnection_count();
    return true;
}

void CommsGsmStack::disconnect(){
    DEBUG_INFO_LN("Modem disconnecting");
    modem.gprsDisconnect();
    modem.poweroff();

}

bool CommsGsmStack::connect_to_network(){
    DEBUG_INFO_LN("Connecting to network");
    if(!modem.waitForNetwork()){
        increment_reconnection_count();
        DEBUG_INFO_LN("Unable to initialize network registration");
        return false;
    }
    return true;

}

bool CommsGsmStack::connect_to_gprs(){
    DEBUG_INFO_LN("Connecting to GPRS");
    if (!modem.gprsConnect(APN_NAME, CELLULAR_USER_NAME, CELLULAR_PASSWORD)){
        increment_reconnection_count();
        DEBUG_INFO_LN("Cannot connect to GPRS");
        return false;

    }
    return true;
}

void CommsGsmStack::increment_reconnection_count(){
    modem_connection_attempt_count ++;
    return;
}

void CommsGsmStack::reset_reconnection_count(){
    modem_connection_attempt_count = 0;
    return;
}
CommsIpstack * CommsGsmStack::get_instance(){
    static CommsGsmStack gsm_stack;
    return &gsm_stack;
}

IPStack* CommsGsmStack::get_ipstack(){
    return &ipstack;
}