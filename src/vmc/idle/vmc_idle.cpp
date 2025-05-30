#include "vmc_idle.h"
#include "vmc/vmc_errors.h"
#include "sensors/rtc/rtc.h"
#include "storage/storage.h"
#include "vmc/vmc_data.h"
#include "vmc/vmc_flags.h"
#include "vmc/dispensing/vmc_dispensing.h"

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
    check_idle_page_flags();
    /* run sensors */
    run_sensors();

    /* process funds */
    

    /* send check_messages, to be removed later: */
    if (millis() - tick_time > TICK_INTERVAL){
      check_event_t ev;
      ev.rss = Comms::get_instance()->get_rss();
      publish_check_event(&ev);
      DEBUG_INFO(_time.hour);
      DEBUG_INFO(":");
      DEBUG_INFO_LN(_time.minutes);

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
    /* time: */
    systemTime::get_default_instance()->getTime(_time);
    
}

void vmc_idle::check_idle_page_flags(){
    // check and clear flags from the idle page in order to clear flags and to transition to the dispense state:
    if (check_vmc_flag(VMC_DISPENSE_TAP1)){
        clear_vmc_flag(VMC_DISPENSE_TAP1);
        this->vmc->set_state(vmc_dispensing::get_default_instance());
        vmc_dispensing::get_default_instance()->set_initial_tap(DISPENSE_TAP_1);
    } else if (check_vmc_flag(VMC_DISPENSE_TAP2)){
        clear_vmc_flag(VMC_DISPENSE_TAP2);
        this->vmc->set_state(vmc_dispensing::get_default_instance());
        vmc_dispensing::get_default_instance()->set_initial_tap(DISPENSE_TAP_2);
    } else if (check_vmc_flag(VMC_DISPENSE_TAP3)){
        clear_vmc_flag(VMC_DISPENSE_TAP3);
        this->vmc->set_state(vmc_dispensing::get_default_instance());
        vmc_dispensing::get_default_instance()->set_initial_tap(DISPENSE_TAP_3);
    } else if (check_vmc_flag(VMC_DISPENSE_TAP4)){
        clear_vmc_flag(VMC_DISPENSE_TAP4);
        this->vmc->set_state(vmc_dispensing::get_default_instance());
        vmc_dispensing::get_default_instance()->set_initial_tap(DISPENSE_TAP_4);
    }
}
