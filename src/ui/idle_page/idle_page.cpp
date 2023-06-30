#include "idle_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"




idlePage::idlePage(){
    id = "IDLE PAGE";
}

int idlePage::load(){
   net_check_timer = millis();
   clear_displays();

   return 0;
}

int idlePage::update(){
    if(!loaded){
        load();
        loaded = true;
    }
    
    display_time();
    

    if(!uiInput::get_default_instance()->door_closed()){
        display_info("DOOR OPEN");

        // we no longer need to check for tap button presses or the internet connection, simply return
        return -1;
    }

    if ((millis() - net_check_timer) >= IDLE_NET_CHECK_TIMER_INTERVAL){

        
        if(check_vmc_flag(VMC_NET_CONNECTED)){

            display_net_connected();

        } else{

            display_net_notConnected();
            // ordinarily should return , since machine should not operate without internet
        }
        net_check_timer = millis();
        
    }


    
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        get_buzzer()->beep(20);
    }

   
    return 0;
}

idlePage* idlePage::get_default_instance(){
    static idlePage page;
    return &page;
}
