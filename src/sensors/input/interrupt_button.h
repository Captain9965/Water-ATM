#pragma once
#include "common/common.h"

#define BUTTON_DEBOUNCE_TIME    20

class buttonInterrupt{
    public:
        buttonInterrupt(uint32_t pin, callback_function_t callback, uint8_t mode, uint8_t interrupt);
        ~buttonInterrupt();
        void enable();
        void disable();
        // bool is_pressed();
    private:
        uint32_t _pin;
        uint8_t _interrupt;
        callback_function_t _callback;
        long long previousPress = 0;
    
};