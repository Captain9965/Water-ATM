#pragma once
#include "common/common.h"
#define DEBOUNCE_DELAY  50

class pollingButton{
    public:
        pollingButton(uint32_t pin, uint8_t mode);
        bool is_pressed();
        void enable(){
            enabled = true;
        };
        void disable(){
            enabled = false;
        };
    private:
        uint32_t _pin;
        uint8_t _mode;
        long long last_debounce_time;
        int last_button_state;
        int button_state;
        bool enabled = true;
};