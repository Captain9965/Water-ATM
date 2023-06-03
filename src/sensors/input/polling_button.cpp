#include "polling_button.h"

pollingButton::pollingButton(uint32_t pin, uint8_t mode):_pin(pin), _mode(mode){
    pinMode(_pin, _mode);
    return;
};

bool pollingButton::is_pressed(){
    if (_mode == INPUT_PULLUP){
        return !digitalRead(_pin);
    }
    return digitalRead(_pin);
}
