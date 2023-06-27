#include "vmc_idle.h"
#include "vmc/vmc_errors.h"
#include "sensors/rfid/rfid.h"
#include "sensors/rtc/rtc.h"
#include "storage/storage.h"
#include "vmc/vmc_data.h"

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
      DEBUG_INFO(_time.hour);
      DEBUG_INFO(":");
      DEBUG_INFO_LN(_time.minutes);
      storage::get_default_instance()->printSDCardContent();
      double CalibrationVal = 0.0;
      Calibration::get_default_instance()->get(&CalibrationVal);
      
      DEBUG_INFO("Calibration is "); DEBUG_INFO_LN(String(CalibrationVal));

      quantities_t quantities;

      Quantities::get_default_instance()->get(&quantities);
      DEBUG_INFO("Quantity 1 -> "); DEBUG_INFO_LN(String(quantities.quantity1));
      DEBUG_INFO("Quantity 2 -> "); DEBUG_INFO_LN(String(quantities.quantity2));
      DEBUG_INFO("Quantity 3 -> "); DEBUG_INFO_LN(String(quantities.quantity3));
      DEBUG_INFO("Quantity 4 -> "); DEBUG_INFO_LN(String(quantities.quantity4));
      DEBUG_INFO("Quantity 5 -> "); DEBUG_INFO_LN(String(quantities.quantity5));
      DEBUG_INFO("Quantity 6 -> "); DEBUG_INFO_LN(String(quantities.quantity6));

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
    /* RFID : */
    String read_string = RFID::get_default_instance()->read_uid();
    if(isServiceTag(read_string)){

        DEBUG_INFO_LN("Incrementing Admin Cash..");
        uint32_t adminCash = 0;
        AdminCash::get_default_instance()->get(&adminCash);
        AdminCash::get_default_instance()->set(++adminCash);
        double CalibrationVal = 0.0;
        Calibration::get_default_instance()->get(&CalibrationVal);
        Calibration::get_default_instance()->set(++CalibrationVal);
        quantities_t quantities = {};
        Quantities::get_default_instance()->get(&quantities);
        ++ quantities.quantity1;
        ++ quantities.quantity2;
        Quantities::get_default_instance()->set(quantities);
    }

    /* time: */
    systemTime::get_default_instance()->getTime(_time);
    
}
bool vmc_idle::isServiceTag(String &uid){
    if (uid.substring(1) == SERVICE_TAG)
    {
        return true;
    }
    return false;
   
}
