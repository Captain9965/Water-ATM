#include "booting_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"




bootingPage::bootingPage(){
    id = "BOOTING PAGE";
}

int bootingPage::load(){
   screen_boot_timer = millis();
   net_check_timer = millis();
    return 0;
}

int bootingPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }

    if ((millis() - net_check_timer) >= BOOT_NET_CHECK_TIMER_INTERVAL){
        check_vmc_flag(VMC_NET_CONNECTED) ? display_net_connected() : display_net_notConnected();
        net_check_timer = millis();
    }
    if(uiInput::get_default_instance()->joystick_button_pressed()){
        DEBUG_INFO_LN("Button press detected!!");
    }
    if (uiInput::get_default_instance()->joystick_up()){
        DEBUG_INFO_LN("Joystick up!!");
    }
    if (uiInput::get_default_instance()->joystick_down()){
        DEBUG_INFO_LN("Joystick down!!");
    }
    if (uiInput::get_default_instance()->joystick_left()){
        DEBUG_INFO_LN("Joystick left!!");
    }
    if (uiInput::get_default_instance()->joystick_right()){
        DEBUG_INFO_LN("Joystick right!!");
    }
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        get_display1()->clear();
        get_display1()->setCursor(2, 2);
        get_display1()->print("Tap ");
        get_display1()->setCursor(2, 3);
        get_display1()->print(tap);
        DEBUG_INFO_LN("Button pressed");
    }

    input_flags_t quantity;
    if(uiInput::get_default_instance()->quantity_button_pressed(quantity)){
        get_display1()->clear();
        get_display1()->setCursor(2, 2);
        get_display1()->print("Quantity ");
        get_display1()->setCursor(2, 3);
        get_display1()->print(quantity);
        DEBUG_INFO_LN("Button pressed");
    }

    if(!uiInput::get_default_instance()->door_closed()){
        get_display1()->clear();
        get_display1()->setCursor(2, 2);
        get_display1()->print("Door Open");
    }
   
    return 0;
}

bootingPage* bootingPage::get_default_instance(){
    static bootingPage page;
    return &page;
}
