#include "booting_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"




bootingPage::bootingPage(){
    id = "BOOTING PAGE";
}

int bootingPage::load(){
   screen_boot_timer = millis();
   net_check_timer = millis();
   uiInput::get_default_instance()->enable_joystick_button(); 
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
   
    return 0;
}

bootingPage* bootingPage::get_default_instance(){
    static bootingPage page;
    return &page;
}
