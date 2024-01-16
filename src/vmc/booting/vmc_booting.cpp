#include "vmc_booting.h"
#include "../idle/vmc_idle.h"
#include "../vmc_flags.h"
#include "sensors/rfid/rfid.h"
#include "sensors/rtc/rtc.h"
#include "storage/storage.h"
#include "vmc/vmc_data.h"


vmc_booting::vmc_booting(){
    id = "BOOTING";
}

int vmc_booting::start(){
    started = true;
    DEBUG_INFO_LN("booting state starting..");

     /* clear both system flags */
    vmc_flags_init();

    _error = init_maintask();
   if (_error != VMC_OK){
    DEBUG_INFO_LN(" Main task failed to initialize");
    set_vmc_flag(VMC_MEMORY_ERROR);
    return VMC_ERROR_MAINTASK_INIT;
   }

   _error = init_comms();
   if (_error != VMC_OK){
    DEBUG_INFO_LN(" Comms failed to initialize");
    set_vmc_flag(VMC_MEMORY_ERROR);
    return VMC_ERROR_COMMS_INIT;
   }

   _error = init_input_task();
   if (_error != VMC_OK){
    DEBUG_INFO_LN("Input task failed to initialize");
    /* go to error state: */
    return VMC_ERROR_INPUT_INIT;
   }

    /* initialize booting timer: */
    return 0;
}

int vmc_booting::run(){
    if(!started){
        start();
    }
    /* Note that RFID must be initialized before storage.
        Otherwise RFID will not work!!
    */
   _error = init_sensors();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Sensors init fail");
        /*change state to error state*/
   }

   _error = init_storage();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Storage failed to initialize");
        /*change state to error state*/
        set_vmc_flag(VMC_STORAGE_ERROR);
   }

   _error = init_settings();
   if (_error != VMC_OK){
        DEBUG_INFO_LN("Settings init fail");
        /*change state to error state*/
        
   }

   _error = init_actuators();
   if (_error != VMC_OK){
    DEBUG_INFO_LN("Actuators init fail");
    /* change state to error state */
   }

   _error = init_services();
   if (_error != VMC_OK){
    DEBUG_INFO_LN("Actuators init fail");
    /* change state to error state*/
   }
//    system_time_t time;
//    time.day = 30;
//    time.hour = 16;
//    time.seconds = 00;
//    time.year = 23;
//    time.minutes = 24;
//    time.month = 6;
//     systemTime::get_default_instance()->setTime(time);
    stop();
    return 0;
}

int vmc_booting::stop(){
    DEBUG_INFO_LN("booting state stopping...");
    /* send info to ui*/
    set_vmc_flag(VMC_BOOTING_DONE);
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
    if(storage::get_default_instance()->init()){
        return VMC_OK;
    }

    return VMC_ERROR_STORAGE_INIT;
}

/*load settings to memory*/

vmc_error_t vmc_booting::init_settings(){
    DEBUG_INFO_LN("init_settings");
    /* settings depend on storage*/
    vmc_error_t return_val = VMC_OK;
    if (!AdminCash::get_default_instance()->load()){
        DEBUG_INFO_LN("Admin Cash failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }

    if(!Tariff::get_default_instance()->load()){
        DEBUG_INFO_LN("Tariff failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }

    if(!Calibration::get_default_instance()->load()){
        DEBUG_INFO_LN("Calibration failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }

    if(!Quantities::get_default_instance()->load()){
        DEBUG_INFO_LN("Quantities failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }

    if(!FlowCalculationTime::get_default_instance()->load()){
        DEBUG_INFO_LN("Flow calculation time failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }


   /* create all setting instances here: */
    return return_val;
}

vmc_error_t vmc_booting::init_sensors(){
    DEBUG_INFO_LN("init_sensors");

    /* initialize rfid: */
    RFID::get_default_instance()->init();

    /* initialize rtc */
    systemTime::get_default_instance()->init();

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

vmc_error_t vmc_booting::init_input_task(){
    DEBUG_INFO_LN("init_input_task");
    _input_taskhandle = get_input_task();

    if(_input_taskhandle == nullptr){
        DEBUG_INFO_LN("_input_taskhandle == nullptr");
        return VMC_ERROR_INPUT_INIT;
    }
    return VMC_OK;
}