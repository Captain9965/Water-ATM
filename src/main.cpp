
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
    vmc->set_state(vmc_booting::get_default_instance());
    DEBUG_INFO_LN("*****************Starting Application***************");

    vmc->start();
}

void loop() {
    /* empty */
}

