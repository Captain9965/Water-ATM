#include "idle_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"
#include "../dispensing_page/dispensing_page.h"
#include "../admin_page/admin_page.h"
#include "../service_page/service_page.h"
#include "sensors/rfid/rfid.h"




idlePage::idlePage(){
    id = "IDLE PAGE";
}

int idlePage::load(){
   get_display2()->clear();
   get_display1()->clear();
   if(!check_vmc_flag(VMC_NET_CONNECTED)){
        display_net_notConnected();
    }
   uiInput::get_default_instance()->disable_quantity_buttons();
   uiInput::get_default_instance()->disable_joystick_button();
   uiInput::get_default_instance()->enable_tap_buttons();
   return 0;
}

int idlePage::update(){
    if(!loaded){
        load();
        loaded = true;
    }
    
    display_time();
    
    display_network_strength();

    if(!uiInput::get_default_instance()->door_closed()){
        display_info("DOOR OPEN");

        // we no longer need to check for tap button presses or the internet connection, simply return
        return -1;
    }

    if ((millis() - net_check_timer) >= IDLE_NET_CHECK_TIMER_INTERVAL){
        // DEBUG_INFO_LN("Checking network connection...");
        if(check_vmc_flag(VMC_NET_CONNECTED)){

            display_machine_ready(_clear_display);
            _clear_display = false;

        } else{
            /* for now we can bypass this condition, ideally, we should return and not even check for button presses altogether*/
            //    display_machine_ready(false);
            get_display1()->clear();
            display_net_notConnected();
            _clear_display = true;

        }
        net_check_timer = millis();
        
    }

    if (!check_vmc_flag(VMC_NET_CONNECTED)){
        return -1;
    }


    
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        get_buzzer()->beep(20);
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

    String uid = RFID::get_default_instance()->read_uid();
        if (is_admin_tag(uid)){
            get_buzzer()->beep(20);
            this->ui->set_page(adminPage::get_default_instance());
        } else if(is_service_tag(uid)){
            get_buzzer()->beep(20);
            this->ui->set_page(servicePage::get_default_instance());
        } else{
            if(uid.length() > 1){
                DEBUG_INFO_LN(uid);
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

bool idlePage::is_admin_tag(String &uid){
    //get admin card uid:
    String admin_uid;
    AdminCard::get_default_instance()->get(&admin_uid);
    if (uid.substring(1) == admin_uid)
    {
        return true;
    }
    return false;
}

bool idlePage::is_service_tag(String &uid){
    //get service card uid:
    String service_uid;
    ServiceCard::get_default_instance()->get(&service_uid);
    if (uid.substring(1) == service_uid)
    {
        return true;
    }
    return false;
}