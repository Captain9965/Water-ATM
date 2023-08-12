#pragma once
#include "common/common.h"
#define DEBOUNCE_DELAY  50


typedef enum button_state{

    BUTTON_READING,
    BUTTON_WAITING_RELEASE,

}button_state_t;

class pollingButton{
    public:
        pollingButton(uint32_t pin, uint8_t mode);
        bool is_pressed();
        int get_button_state();
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
        button_state_t reading_state = BUTTON_READING;
};