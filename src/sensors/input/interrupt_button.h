#pragma once
#include "common/common.h"

#define BUTTON_DEBOUNCE_TIME    20

class buttonInterrupt{
    public:
        buttonInterrupt(uint32_t button, callback_function_t callback);
        ~buttonInterrupt();
        bool is_pressed();
    private:
        void ISR();
        long long previousPress = 0;
    
};