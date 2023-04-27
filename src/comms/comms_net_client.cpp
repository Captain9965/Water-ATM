#include "comms_net_client.h"

/**
 * @todo: 
 * Remove these delays!!!!
 * 
 */
void GsmClient::init(){
    DEBUG_INFO_LN("Powering on modem");
    GSMSerial.begin(SYSTEM_GSM_BAUDRATE);
    wait_ms(100);
    pinMode(SYSTEM_GSM_POWER_KEY, OUTPUT);
    modem_power_on();
}

void GsmClient::modem_power_on(){
    DEBUG_INFO_LN("Turning on modem");
    modem_cycle_power();
    DEBUG_INFO("Found Modem IMEI: ");
    DEBUG_INFO_LN(get_modem_imei());
}
void GsmClient::modem_power_off(){
    DEBUG_INFO_LN("Turning off modem");
    modem.poweroff();
    wait_ms(300);
}

void GsmClient::modem_cycle_power(){
    digitalWrite(SYSTEM_GSM_POWER_KEY, HIGH);
    wait_ms(3000);
    digitalWrite(SYSTEM_GSM_POWER_KEY, LOW);
    wait_ms(100);
}

String GsmClient::get_modem_imei(){
    return modem.getIMEI();
}

int16_t GsmClient::get_signal_strength(){
    return modem.getSignalQuality();
}

String GsmClient::get_network_operator(){
    return modem.getOperator();
}

bool GsmClient::is_GPRS_connected(){
    return modem.isGprsConnected();
}

bool GsmClient::is_network_connected(){
    return modem.isNetworkConnected();
}
bool GsmClient::connected(){
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

bool GsmClient::connect(){
    if (modem_connection_attempt_count > 0){
        DEBUG_INFO("Reconnecting to network, restarting modem Reconnection count is -> ");

        /**
         * @todo: maybe reset mcu after a number of reconnection attempts
         * 
         */
        DEBUG_INFO_LN(modem_connection_attempt_count);
        modem.restart();
    }
    modem.init();
    if (!connect_to_network() || !connect_to_gprs()){
        return false;
    }
    reset_reconnection_count();
    return true;
}

void GsmClient::disconnect(){
    DEBUG_INFO_LN("Modem disconnecting");
    modem.gprsDisconnect();
    modem.poweroff();

}

bool GsmClient::connect_to_network(){
    DEBUG_INFO_LN("Connecting to network");
    if(!modem.waitForNetwork()){
        increment_reconnection_count();
        DEBUG_INFO_LN("Unable to initialize network registration");
        return false;
    }
    return true;

}

bool GsmClient::connect_to_gprs(){
    DEBUG_INFO_LN("Connecting to GPRS");
    if (!modem.gprsConnect(APN_NAME, CELLULAR_USER_NAME, CELLULAR_PASSWORD)){
        increment_reconnection_count();
        DEBUG_INFO_LN("Cannot connect to GPRS");
        return false;

    }
    return true;
}

void GsmClient::increment_reconnection_count(){
    modem_connection_attempt_count ++;
    return;
}

void GsmClient::reset_reconnection_count(){
    modem_connection_attempt_count = 0;
    return;
}
CommsClient * GsmClient::get_instance(){
    static GsmClient gsm_stack;
    return &gsm_stack;
}

Client* GsmClient::get_client(){
    return &tinyGSMClient;
}