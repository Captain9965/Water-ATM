#pragma once
#include "vmc/vmc.h"
#include "comms/comms.h"
/* state booting */
class vmc_booting: public State{
    public:
        vmc_booting();
        int start() override;
        int stop () override;
        int run() override;
        static vmc_booting* get_default_instance();
    private:
        vmc_error_t _error = VMC_OK;
        /* storage*/
        /* comms taskhandle:  */
        BaseType_t * _comms_taskhandle = nullptr;
        BaseType_t * _main_taskhandle = nullptr;
        BaseType_t * _input_taskhandle = nullptr;
        vmc_error_t init_maintask();
        vmc_error_t init_input_task();
        vmc_error_t init_storage();
        vmc_error_t init_settings();
        vmc_error_t init_sensors();
        vmc_error_t init_actuators();
        vmc_error_t init_services();
        vmc_error_t init_comms();
        /* state timer*/
};