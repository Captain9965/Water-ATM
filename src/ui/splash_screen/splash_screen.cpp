#include "splash_screen.h"
#include "../booting_page/booting_page.h"



splashScreenPage::splashScreenPage(){
    id = "SPLASH SCREEN PAGE";
}

int splashScreenPage::load(){

    get_display1()->setCursor(1, 1);
    get_display1()->print("Augmented Taps");

    get_display2()->setCursor(4, 1);
    get_display2()->print("Version: ");

    get_display2()->setCursor(4, 2);
    get_display2()->print("1.0.0 ");

  
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
        this->ui->set_page(bootingPage::get_default_instance());
        
    }
    return 0;
}

splashScreenPage* splashScreenPage::get_default_instance(){
    static splashScreenPage page;
    return &page;
}
