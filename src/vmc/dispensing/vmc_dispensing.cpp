#include "vmc_dispensing.h"
#include "../idle/vmc_idle.h"


vmc_dispensing::vmc_dispensing(){
    id = "DISPENSING";
}
int vmc_dispensing::start(){
    started = true;
    DEBUG_INFO_LN("Dispensing state starting..");

    
    return 0;
}

void vmc_dispensing::set_initial_tap(tap_selection_t tap){
    _initialTap = tap;
}

int vmc_dispensing::run(){
    if(!started){
        start();
    }

    /* run sensors */
    run_sensors();
  
    /* process funds */
    
    
    /* check whether all dispensing systems are done and go back to idle state: */
    if(!_dispense_service){
        // create instance then start
        _dispense_service = new DispenseSystem(_initialTap);
        _dispense_service->start();
    }
    dispensing_state_t state = _dispense_service->run();
    if (state == DISPENSING_EXIT){
        delete _dispense_service;
        _dispense_service = nullptr;
        stop();
    }
    return 0;
}

int vmc_dispensing::stop(){
    /* stop and return to idle state: */
    DEBUG_INFO_LN("Dispensing state stopping...");
    this->vmc->set_state(vmc_idle::get_default_instance());
    return 0;
}

vmc_dispensing* vmc_dispensing::get_default_instance(){
    static vmc_dispensing vmc_state;
    return &vmc_state;
}

DispenseSystem * vmc_dispensing::get_dispense_instance(){
    return _dispense_service;
}

void vmc_dispensing::run_sensors(){
  
}