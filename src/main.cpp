
#include "main.h"

void comms_task(void *pvParameters);
void vmc_task(void *pvParameters);
void setup() {
    int error = 0;
    DEBUG_INIT(&error);

    if (error < 0){
      while (1)
      {
        /* maybe reset mcu??*/
      }
    }
    DEBUG_INFO_LN("*****************Starting Application***************");
    xTaskCreate(comms_task, "comms_task", 500, nullptr, 0, nullptr);
    xTaskCreate(vmc_task, "vmc_task", 500, nullptr, 0, nullptr);
    vTaskStartScheduler();
}

void loop() {
    /* empty */
}

void comms_task(void * pvParameters){
  Comms::get_instance()->run();
}

void vmc_task(void * pvParameters){
  long long tick_time = millis();
  while(1){

    if (millis() - tick_time > TICK_INTERVAL){
      check_event_t ev;
      publish_check_event(&ev);
      DEBUG_INFO_LN("Main VMC tick");
      #ifdef MEM_DEBUG
      stack_debug();
      #endif
      tick_time = millis();
    }
    
    wait_ms(2000);
  } 
}
