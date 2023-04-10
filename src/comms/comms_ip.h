#pragma once
#include "common/common.h"
#include <IPStack.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
#endif

class CommsIpstack{
    public:
        CommsIpstack(){};
        ~CommsIpstack(){};
        virtual void init() = 0;
        virtual bool connect() = 0;
        virtual bool connected() = 0;
        virtual int16_t get_signal_strength() = 0;
        virtual IPStack * get_ipstack() = 0;
    
};

class CommsGsmStack: public CommsIpstack{
    public:
        CommsGsmStack(){};
        ~CommsGsmStack(){};
        void init()override;
        void modem_power_on();
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
        static CommsIpstack * get_instance();
        IPStack * get_ipstack()override;
    private:
        /* declare modem serial interface */
        HardwareSerial GSMSerial = HardwareSerial(SYSTEM_GSM_RX_PIN, SYSTEM_GSM_TX_PIN);

        #ifdef DUMP_AT_COMMANDS
        StreamDebugger debugger = StreamDebugger(GSMSerial, SerialDebug);
        TinyGsm modem = TinyGsm(debugger);
        #else
        TinyGsm modem = TinyGsm(GSMSerial);
        #endif
        
        TinyGsmClient tinyGSMClient = TinyGsmClient(modem);
        IPStack ipstack = IPStack(tinyGSMClient);
        int8_t modem_connection_attempt_count = 0;
};