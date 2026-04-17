#pragma once
#include "vmc/vmc.h"
#include "comms/comms.h"

class vmc_booting: public State{
    public:
        vmc_booting();
        int start() override;
        int stop()  override;
        int run()   override;
        static vmc_booting* get_default_instance();
    private:
        vmc_error_t _error = VMC_OK;
        vmc_error_t init_comms();
        vmc_error_t init_storage();
        vmc_error_t init_settings();
        vmc_error_t init_sensors();
        vmc_error_t init_actuators();
        vmc_error_t init_services();
        // Returns true and begins the OTA session when a pending update is found
        bool        check_for_ota_update();
};
