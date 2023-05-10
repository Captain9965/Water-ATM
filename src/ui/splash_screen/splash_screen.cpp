#include "splash_screen.h"



splashScreenPage::splashScreenPage(){
    id = "SPLASH SCREEN PAGE";
}

int splashScreenPage::load(){
   screen_boot_timer = millis();
    return 0;
}

int splashScreenPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }
    if (millis() - screen_boot_timer > SPLASH_SCREEN_DELAY){
        /* change page here after booting*/
        screen_boot_timer = millis();
    }
    return 0;
}

splashScreenPage* splashScreenPage::get_default_instance(){
    static splashScreenPage page;
    return &page;
}
