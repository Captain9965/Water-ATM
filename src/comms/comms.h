#pragma once
#include "mqtt.h"

/* comms class */

class Comms{
    public:
        Comms();
        ~Comms();
        /**
            * run comms logic , blocks indefinitely, put in thread instead
            * - checks & maintains network connection
            * - registers event publishers
            * - dispatches comms event queue
        */
       void run(void);
       void init(void);
       static Comms * get_instance(void);
    //    bool is_net_connected(void);
       int16_t get_rss(void);
    private:
        // void check_net_connection(void);
        // void check_mqtt_connection(void);

        /* declare my interval timers */

        /* declare my event queue*/

        /* declare my comms mqtt client class */
        CommsMQTTClient * _mqtt_client = nullptr;
        CommsIpstack * _ipstack = nullptr;

        /* send timer: experimental*/
        long long last_send_time = millis();

        
};