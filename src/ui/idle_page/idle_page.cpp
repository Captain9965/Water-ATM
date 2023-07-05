#include "idle_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"
#include "../dispensing_page/dispensing_page.h"




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
            /* for now we can bypass this condition, ideally, we should return and not even check for button presses altogether*/
               display_net_connected();
            // display_net_notConnected();
            // return -1;
        }
        net_check_timer = millis();
        
    }


    
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        // get_buzzer()->beep(20);
        switch (tap){
            case TAP_1_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP1);
                stop();
                break;
            case TAP_2_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP2);
                stop();
                break;
            case TAP_3_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP3);
                stop();
                break;
            case TAP_4_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP4);
                stop();
                break;
            default:
                break;
        }
    }

   
    return 0;
}

int idlePage::stop(){
    this->ui->set_page(dispensingPage::get_default_instance());
    return 0;
}

idlePage* idlePage::get_default_instance(){
    static idlePage page;
    return &page;
}
