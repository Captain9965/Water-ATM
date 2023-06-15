#include "interrupt_button.h"


buttonInterrupt::buttonInterrupt(uint32_t pin, callback_function_t callback, uint8_t mode, uint8_t interrupt): _callback(callback){
    _pin = pin;
    _interrupt = interrupt;
    pinMode(_pin, mode);
    return;
}

buttonInterrupt::~buttonInterrupt(){
    disable();
}

void buttonInterrupt::enable(){
    attachInterrupt(digitalPinToInterrupt(_pin), _callback, _interrupt);
    return;
}

void buttonInterrupt::disable(){
    detachInterrupt(_pin);
    return;
}

