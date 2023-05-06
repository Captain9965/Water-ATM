#pragma once
#include "common/common.h"
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
#endif

class CommsClient{
    public:
        CommsClient(){};
        ~CommsClient(){};
        virtual void init() = 0;
        virtual bool connect() = 0;
        virtual bool connected() = 0;
        virtual int16_t get_signal_strength() = 0;
        virtual void disconnect()= 0;
        virtual Client * get_client() = 0;
    
};

class GsmClient: public CommsClient{
    public:
        GsmClient(){};
        ~GsmClient(){};
        void init()override;
        void disconnect()override;
        void modem_power_on();
        void modem_power_off();
        String get_modem_imei();
        int16_t get_signal_strength()override;
        String get_network_operator();
        bool is_network_connected();
        bool is_GPRS_connected();
        bool connect()override;
        bool connected()override;
        bool connect_to_network();
        bool connect_to_gprs();
        void increment_reconnection_count();
        void reset_reconnection_count();
        static CommsClient * get_instance();
        Client * get_client()override;
    private:
        /* declare modem serial interface */
        void modem_cycle_power();
        HardwareSerial GSMSerial = HardwareSerial(SYSTEM_GSM_RX_PIN, SYSTEM_GSM_TX_PIN);

        #ifdef DUMP_AT_COMMANDS
        StreamDebugger debugger = StreamDebugger(GSMSerial, SerialDebug);
        TinyGsm modem = TinyGsm(debugger);
        #else
        TinyGsm modem = TinyGsm(GSMSerial);
        #endif
        
        TinyGsmClient tinyGSMClient = TinyGsmClient(modem);
        int8_t modem_connection_attempt_count = 0;
};