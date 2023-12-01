#include "error_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"


errorPage::errorPage(const char * error_msg){
    id = "BOOTING PAGE";
    _error_message = error_msg;
}

int errorPage::load(){
   screen_boot_timer = millis();
   get_display1()->clear();
   get_display2()->clear();
    return 0;
}

int errorPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }


    get_display1()->setCursor(1, 1);
    get_display1()->print("CRITICAL ERROR: ");

    get_display1()->setCursor(1, 2);
    get_display1()->print(_error_message);

    get_display2()->setCursor(1, 1);
    get_display2()->print("PLEASE REBOOT");

    
   
   
    return 0;
}

errorPage* errorPage::get_default_instance(const char * error_msg){
    static errorPage page(error_msg);
    return &page;
}
