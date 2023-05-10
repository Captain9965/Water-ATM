
#include "main.h"


void setup() {
    int error = 0;
    DEBUG_INIT(&error);

    if (error < 0){
      while (1)
      {
        /* maybe reset mcu??*/
      }
    }
    VMC * vmc = VMC::get_default_instance();

    /* user interface: */
    UI * user_interface = UI::get_default_instance();
    user_interface->set_page(splashScreenPage::get_default_instance());

    vmc->set_state(vmc_booting::get_default_instance());
    DEBUG_INFO_LN("*****************Starting Application***************");

    vmc->start();
    user_interface->init();
    
    vTaskStartScheduler();
}

void loop() {
    /* empty */
}

