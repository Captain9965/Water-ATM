#include "interrupt_button.h"


buttonInterrupt::buttonInterrupt(uint32_t pin, callback_function_t callback): _callback(callback){
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
    return;
}

buttonInterrupt::~buttonInterrupt(){
    disable();
}

void buttonInterrupt::enable(){
    attachInterrupt(digitalPinToInterrupt(_pin), _callback, FALLING);
    return;
}

void buttonInterrupt::disable(){
    detachInterrupt(_pin);
    return;
}

