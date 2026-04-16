#include "main.h"

void setup() {
    int error = 0;
    DEBUG_INIT(&error);

    if (error < 0){
        while (1) { /* halt — debug serial failed */ }
    }

    VMC* vmc = VMC::get_default_instance();

    UI* user_interface = UI::get_default_instance();
    user_interface->set_page(splashScreenPage::get_default_instance());

    vmc->set_state(vmc_booting::get_default_instance());
    DEBUG_INFO_LN("*****************Starting Application***************");

    vmc->start();          // runs vmc_booting::start() → init_comms() → modem serial + A7680 begin
    user_interface->init();
}

void loop() {
    Comms::get_instance()->update();                                  // modem state machine + publish queue
    VMC::get_default_instance()->run();                               // application state machine
    UI::get_default_instance()->update();                             // LCD refresh
    uiInput::get_default_instance()->update_push_button_states();     // button polling
}
