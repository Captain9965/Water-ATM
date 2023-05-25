#include "interrupt_button.h"


buttonInterrupt::buttonInterrupt(uint32_t button, callback_function_t callback){
    pinMode(button, INPUT);
    attachInterrupt(digitalPinToInterrupt(button), callback, CHANGE);
    return;
}

