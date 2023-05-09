#pragma once
#include "vmc/vmc.h"
#include "comms/comms_events.h"
#define TICK_INTERVAL 10000
/* state idle */
class vmc_idle: public State{
    public:
        vmc_idle();
        int start() override;
        int stop() override;
        int run() override;
        static vmc_idle* get_default_instance();
    private:
        void run_sensors();
        long long tick_time = 0;
        
};