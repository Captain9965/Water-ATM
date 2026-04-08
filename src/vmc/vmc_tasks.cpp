#include "vmc_tasks.h"
#include "vmc.h"
#include "ui/ui.h"
#include "ui/ui_input.h"
#include "sensors/input/joystick.h"

/* task handles: */
BaseType_t * get_comms_taskhandle(){
    static BaseType_t comms_taskhandle = xTaskCreate(comms_task, "comms_task", 256, nullptr, configMAX_PRIORITIES - 5, nullptr);
    return & comms_taskhandle;
}

BaseType_t * get_main_taskhandle(){
    static BaseType_t main_taskhandle = xTaskCreate(main_task, "main_task", configMINIMAL_STACK_SIZE * 2.5, nullptr, configMAX_PRIORITIES - 6, nullptr);
    return & main_taskhandle;
}

BaseType_t * get_input_task(){
  static BaseType_t input_taskhandle = xTaskCreate(input_task, "input_task", configMINIMAL_STACK_SIZE , nullptr, configMAX_PRIORITIES - 6, nullptr );
  return & input_taskhandle;
}


void comms_task(void * pvParameters){
  Comms::get_instance()->run();
}

void main_task(void * pvParameters){
  long long tick_time = millis();

  while(1){
    xTaskLastCheckIn[TASK_IDX_MAIN] = xTaskGetTickCount();
    VMC::get_default_instance()->run();
    UI::get_default_instance()->update();
    wait_ms(15);
  } 
}

void input_task(void * pvParameters){
  uiInput * input_instance = uiInput::get_default_instance();
  while (1){
    xTaskLastCheckIn[TASK_IDX_INPUT] = xTaskGetTickCount();
    // input_instance->update_joystick_state();
    input_instance->update_push_button_states();
    wait_ms(20);
  }
}

void watchdog_reset_system(void) {
    DEBUG_INFO_LN("Resetting system");
    NVIC_SystemReset();
}

void watchdog_task(void * pvParameters){

    const TickType_t xInitialTickCount = xTaskGetTickCount();
    for (int i = 0; i < NUM_MONITORED_TASKS; i++) {
        xTaskLastCheckIn[i] = xInitialTickCount;
    }

    const char* task_names[NUM_MONITORED_TASKS] = {
        "Comms Task", "Main Task", "Input Task"
    };

    while(1){
       
        vTaskDelay(WATCHDOG_CHECK_PERIOD_TICKS); 

        TickType_t xCurrentTicks = xTaskGetTickCount();
        BaseType_t xDeadlockDetected = pdFALSE;

        for (int i = 0; i < NUM_MONITORED_TASKS; i++) {
            // Check if the time since the last check-in exceeds the timeout
            if ((xCurrentTicks - xTaskLastCheckIn[i]) > WATCHDOG_TIMEOUT_TICKS) {
                printf("Watchdog Timeout! Task: %s\n", task_names[i]);
                xDeadlockDetected = pdTRUE;
                break; // One failure is enough to reset
            }
        }
        
        if (xDeadlockDetected == pdTRUE) {
            watchdog_reset_system();
        }
    }
}

