#include "vmc_booting.h"
#include "../idle/vmc_idle.h"


vmc_booting::vmc_booting(){
    id = "BOOTING";
}

int vmc_booting::start(){
    started = true;
    DEBUG_INFO_LN("booting state starting..");

    _error = init_maintask();
   if (_error != VMC_OK){
    DEBUG_INFO_LN(" Main task failed to initialize");
    /* change state to error state*/
    return VMC_ERROR_MAINTASK_INIT;
   }

   _error = init_comms();
   if (_error != VMC_OK){
    DEBUG_INFO_LN(" Comms failed to initialize");
    /* change state to error state*/
    return VMC_ERROR_COMMS_INIT;
   }

   vTaskStartScheduler();
    /* initialize booting timer: */
    return 0;
}

int vmc_booting::run(){
    if(!started){
        start();
    }

   _error = init_storage();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Storage failed to initialize");
        /*change state to error state*/
        return VMC_ERROR_STORAGE_INIT;
   }

   _error = init_settings();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Settings init fail");
        /*change state to error state*/
        return VMC_ERROR_SETTINGS_INIT;
   }

   _error = init_sensors();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Sensors init fail");
        /*change state to error state*/
        return VMC_ERROR_SENSORS_INIT;
   }

   _error = init_actuators();
   if (_error != VMC_OK){
    DEBUG_INFO_LN("Actuators init fail");
    /* change state to error state */
    return VMC_ERROR_ACTUATORS_INIT;
   }

   _error = init_services();
   if (_error != VMC_OK){
    DEBUG_INFO_LN("Actuators init fail");
    /* change state to error state*/
    return VMC_ERROR_SERVICES_INIT;
   }

    /* clear both system flags */

    stop();
    return 0;
}

int vmc_booting::stop(){
    DEBUG_INFO_LN("booting state stopping...");
    /* send info to ui*/
    this->vmc->set_state(vmc_idle::get_default_instance());
    return 0;
}

vmc_booting* vmc_booting::get_default_instance(){
    static vmc_booting vmc_state;
    return &vmc_state;
}

/*initialize storage*/
vmc_error_t vmc_booting::init_storage(){

    DEBUG_INFO_LN("init_storage");
   

    return VMC_OK;
}

/*load settings to memory*/

vmc_error_t vmc_booting::init_settings(){
    DEBUG_INFO_LN("init_settings");
    /* settings depend on storage*/

   /* create all setting instances here: */
    return VMC_OK;
}

vmc_error_t vmc_booting::init_sensors(){
    DEBUG_INFO_LN("init_sensors");


    return VMC_OK;
}

vmc_error_t vmc_booting::init_actuators(){
    DEBUG_INFO_LN("init_actuators");

    return VMC_OK;
}

vmc_error_t vmc_booting::init_services(){
    return VMC_OK;
}

vmc_error_t vmc_booting::init_comms(){
    DEBUG_INFO_LN("init_comms");

    _comms_taskhandle = get_comms_taskhandle();
    
    if (_comms_taskhandle == nullptr){
        DEBUG_INFO_LN("_comms_taskhandle = nullptr");
        return VMC_ERROR_COMMS_INIT;
    }

    return VMC_OK;
}

vmc_error_t vmc_booting::init_maintask(){
    DEBUG_INFO_LN("init_maintask");

    _main_taskhandle = get_main_taskhandle();

    if(_main_taskhandle == nullptr){
        DEBUG_INFO_LN("_main_taskhandle == nullptr");
        return VMC_ERROR_MAINTASK_INIT;
    }
    return VMC_OK;
}