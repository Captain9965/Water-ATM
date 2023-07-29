#pragma once
#include "vmc/vmc.h"
#include "vmc/vmc_errors.h"
#include "vmc/vmc_flags.h"
#include "services/dispense_system.h"

/* state dispensing */
class vmc_dispensing: public State{
    public:
        vmc_dispensing();
        int start() override;
        int stop() override;
        int run() override;
        void set_initial_tap(tap_selection_t tap);
        DispenseSystem * get_dispense_instance();
        static vmc_dispensing* get_default_instance();
    private:
        tap_selection_t _initialTap = (tap_selection_t)0;
        void run_sensors();

        /* services*/
        DispenseSystem* _dispense_service = nullptr;


};