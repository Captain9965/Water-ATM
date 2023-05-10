#include "booting_page.h"



bootingPage::bootingPage(){
    id = "BOOTING PAGE";
}

int bootingPage::load(){
   screen_boot_timer = millis();
    return 0;
}

int bootingPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }
   
    return 0;
}

bootingPage* bootingPage::get_default_instance(){
    static bootingPage page;
    return &page;
}
