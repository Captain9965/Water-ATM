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
    DEBUG_LN("*****************Starting Application***************");
}

void loop() {
    Comms::get_instance()->run();
}
