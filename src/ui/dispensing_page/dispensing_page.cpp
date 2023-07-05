#include "dispensing_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"




dispensingPage::dispensingPage(){
    id = "IDLE PAGE";
}

int dispensingPage::load(){
   clear_displays();
   return 0;
}

int dispensingPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }

    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        // get_buzzer()->beep(20);
        switch (tap){
            case TAP_1_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP1);
                break;
            case TAP_2_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP2);
                break;
            case TAP_3_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP3);
                break;
            case TAP_4_BUTTON:
                set_vmc_flag(VMC_DISPENSE_TAP4);
                break;
            default:
                break;
        }
    }

    //which button has been pressed for cancel?. In this case, we have to check for the running flag for dispense activity in each of the taps

    // which button has been pressed for dispense?
    display_button_pressed_for_dispense();

   
    return 0;
}

dispensingPage* dispensingPage::get_default_instance(){
    static dispensingPage page;
    return &page;
}

void dispensingPage::display_button_pressed_for_dispense(){
    if (check_vmc_flag(VMC_DISPENSE_TAP1)){
        clear_vmc_flag(VMC_DISPENSE_TAP1);
        display_select_quantity("TAP 1");
    } else if (check_vmc_flag(VMC_DISPENSE_TAP2)){
        clear_vmc_flag(VMC_DISPENSE_TAP2);
        display_select_quantity("TAP 2");
    } else if (check_vmc_flag(VMC_DISPENSE_TAP3)){
        clear_vmc_flag(VMC_DISPENSE_TAP3);
        display_select_quantity("TAP 3");
    } else if (check_vmc_flag(VMC_DISPENSE_TAP4)){
        clear_vmc_flag(VMC_DISPENSE_TAP4);
        display_select_quantity("TAP 4");
    }
}
