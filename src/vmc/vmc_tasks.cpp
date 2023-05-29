#include "vmc_tasks.h"
#include "vmc.h"
#include "ui/ui.h"
#include "ui/ui_input.h"
#include "sensors/input/joystick.h"

/* task handles: */
BaseType_t * get_comms_taskhandle(){
    static BaseType_t comms_taskhandle = xTaskCreate(comms_task, "comms_task", 500, nullptr, 0, nullptr);
    return & comms_taskhandle;
}

BaseType_t * get_main_taskhandle(){
    static BaseType_t main_taskhandle = xTaskCreate(main_task, "main_task", 500, nullptr, 0, nullptr);
    return & main_taskhandle;
}

BaseType_t * get_input_task(){
  static BaseType_t input_taskhandle = xTaskCreate(input_task, "input_task", 100, nullptr, 1, nullptr );
  return & input_taskhandle;
}


void comms_task(void * pvParameters){
  Comms::get_instance()->run();
}

void main_task(void * pvParameters){
  long long tick_time = millis();

  while(1){
    VMC::get_default_instance()->run();
    UI::get_default_instance()->update();
    wait_ms(200);
  } 
}

void input_task(void * pvParameters){
  uiInput * input_instance = uiInput::get_default_instance();
  while (1){
    input_instance->update_joystick_state();
    wait_ms(100);
  }
}