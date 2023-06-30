#include "booting_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "storage/storage.h"
#include "../error_page/error_page.h"
#include "../idle_page/idle_page.h"




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
    get_display1()->setCursor(1, 1);
    get_display1()->print("Booting...");

    //: Get booting info:
    if (check_vmc_flag(VMC_MEMORY_ERROR)){
        clear_vmc_flag(VMC_MEMORY_ERROR);
        this->ui->set_page(errorPage::get_default_instance("MEMORY ERROR"));
        return -1;
    }

    if (check_vmc_flag(VMC_STORAGE_ERROR)){
        clear_vmc_flag(VMC_STORAGE_ERROR);
        this->ui->set_page(errorPage::get_default_instance("SD CARD ERROR"));
        return -1;
    }

     if (check_vmc_flag(VMC_BOOTING_DONE)){
        clear_vmc_flag(VMC_BOOTING_DONE);
        stop();
    }

    return 0;
}

int bootingPage::stop(){
    this->ui->set_page(idlePage::get_default_instance());
    return 0;
}

bootingPage* bootingPage::get_default_instance(){
    static bootingPage page;
    return &page;
}
