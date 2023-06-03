#pragma once
#include "common/common.h"

class pollingButton{
    public:
        pollingButton(uint32_t pin, uint8_t mode);
        bool is_pressed();
    private:
        uint32_t _pin;
        uint8_t _mode;
        long long previous_press;
};