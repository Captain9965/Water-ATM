#include "vmc_idle.h"
#include "vmc/vmc_errors.h"
#include "sensors/rfid/rfid.h"
vmc_idle::vmc_idle(){
    id = "IDLE";
}
int vmc_idle::start(){
    started = true;
    DEBUG_INFO_LN("Idle state starting..");

  /* assert sensors and initialize timer: */
    tick_time = millis();
    return 0;
}
int vmc_idle::run(){
    if(!started){
        start();
    }
  

    /* run sensors */
    run_sensors();

    /* process funds */
    
    /* check whether the dispensing flag has been set: */

    /* send check_messages, to be removed later: */
    if (millis() - tick_time > TICK_INTERVAL){
      check_event_t ev;
      publish_check_event(&ev);
      DEBUG_INFO_LN("Main VMC tick");
      #ifdef MEM_DEBUG
      stack_debug();
      #endif
      tick_time = millis();
    }

    return 0;
}
int vmc_idle::stop(){
    DEBUG_INFO_LN("Idle state stopping...");
    return 0;
}

vmc_idle* vmc_idle::get_default_instance(){
    static vmc_idle vmc_state;
    return &vmc_state;
}

void vmc_idle::run_sensors(){
    String read_string = RFID::get_default_instance()->read_uid();
    if(isServiceTag(read_string)){

        DEBUG_INFO_LN("Service Tag detected");

    }

    
}
bool vmc_idle::isServiceTag(String &uid){
    if (uid.substring(1) == SERVICE_TAG)
    {
        return true;
    }
    return false;
   
}
