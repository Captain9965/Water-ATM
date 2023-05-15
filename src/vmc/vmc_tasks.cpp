#include "vmc_tasks.h"
#include "vmc.h"
#include "ui/ui.h"

/* task handles: */
BaseType_t * get_comms_taskhandle(){
    static BaseType_t comms_taskhandle = xTaskCreate(comms_task, "comms_task", 500, nullptr, 0, nullptr);
    return & comms_taskhandle;
}

BaseType_t * get_main_taskhandle(){
    static BaseType_t main_taskhandle = xTaskCreate(main_task, "main_task", 500, nullptr, 0, nullptr);
    return & main_taskhandle;
}


void comms_task(void * pvParameters){
  Comms::get_instance()->run();
}

void main_task(void * pvParameters){
  long long tick_time = millis();

  while(1){
    VMC::get_default_instance()->run();
    UI::get_default_instance()->update();
    wait_ms(2000);
  } 
}