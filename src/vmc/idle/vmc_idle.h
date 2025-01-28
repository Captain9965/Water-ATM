#pragma once
#include "vmc/vmc.h"
#include "comms/comms_events.h"
#include "sensors/rtc/rtc.h"

#define TICK_INTERVAL 30000
/* state idle */
class vmc_idle: public State{
    public:
        vmc_idle();
        int start() override;
        int stop() override;
        int run() override;
        static vmc_idle* get_default_instance();
    private:
        void check_idle_page_flags();
        void run_sensors();
        bool isServiceTag(String &uid);
        long long tick_time = 0;
        system_time_t _time;
        
};