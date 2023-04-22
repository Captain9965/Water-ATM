#include "common.h"

char taskListBuffer[120];
void debug_init(int * error){
    SerialDebug.setRx(SYSTEM_SERIAL_DEBUG_RX);
    SerialDebug.setTx(SYSTEM_SERIAL_DEBUG_TX);
    SerialDebug.begin(SYSTEM_DEBUG_BAUDRATE);
    delay(100);
    /* delay for 100 ms*/
    if (!SerialDebug){
        *error = -1;
    }
    return;
}

char * stack_debug(){
    vTaskList(taskListBuffer);
    DEBUG_INFO_LN(F("Task       State   Prio    Stack    Num"));
    DEBUG_INFO_LN(F("*******************************************"));
    DEBUG_INFO(taskListBuffer);
    DEBUG_INFO_LN(F("*******************************************"));
    return taskListBuffer;
}

