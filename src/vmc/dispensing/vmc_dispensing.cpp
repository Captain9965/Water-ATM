#include "vmc_dispensing.h"
#include "../idle/vmc_idle.h"


vmc_dispensing::vmc_dispensing(){
    id = "DISPENSING";
}
int vmc_dispensing::start(){
    started = true;
    DEBUG_INFO_LN("Dispensing state starting..");

    /* sensors*/
    /* state timer */

    /* initialize dispense: */
    // _dispense_service->start();
    // _dispense_service->_set_state(DISPENSING_IDLE);
    // _dispense_service->set_from_event(DISPENSING_IDLE);
    return 0;
}
int vmc_dispensing::run(){
    if(!started){
        start();
    }
   /* continue running debug..*/

    /* run sensors */
    run_sensors();
  
    /* process funds */
    
    
    /* check whether dispensing is done and go back to idle state: */
    return 0;
}
int vmc_dispensing::stop(){
    DEBUG_INFO_LN("Dispensing state stopping...");
    return 0;
}

vmc_dispensing* vmc_dispensing::get_default_instance(){
    static vmc_dispensing vmc_state;
    return &vmc_state;
}

void vmc_dispensing::run_sensors(){
  
}