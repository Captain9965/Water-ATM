#include "idle_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"
#include "../dispensing_page/dispensing_page.h"




idlePage::idlePage(){
    id = "IDLE PAGE";
}

int idlePage::load(){
   get_display2()->clear();
   uiInput::get_default_instance()->disable_quantity_buttons();
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

            display_machine_ready(false);

        } else{
            /* for now we can bypass this condition, ideally, we should return and not even check for button presses altogether*/
               display_machine_ready(false);
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
                stop(VMC_DISPENSE_TAP1);
                break;
            case TAP_2_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP2);
                stop(VMC_DISPENSE_TAP2);
                break;
            case TAP_3_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP3);
                stop(VMC_DISPENSE_TAP3);
                break;
            case TAP_4_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP4);
                stop(VMC_DISPENSE_TAP4);
                break;
            default:
                break;
        }
    }

   
    return 0;
}

int idlePage::stop(vmc_flags_t tap){
    this->ui->set_page(dispensingPage::get_default_instance(tap));
    return 0;
}

idlePage* idlePage::get_default_instance(){
    static idlePage page;
    return &page;
}
