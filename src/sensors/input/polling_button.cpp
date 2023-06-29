#include "polling_button.h"

pollingButton::pollingButton(uint32_t pin, uint8_t mode):_pin(pin), _mode(mode){
    pinMode(_pin, _mode);
    if (_mode == INPUT_PULLUP){
        last_button_state = HIGH;
    } else{
        last_button_state = LOW;
    }
    return;
};

bool pollingButton::is_pressed(){
    int reading = digitalRead(_pin);

    if(reading != last_button_state){
        last_debounce_time = millis();
    }

    if (millis() - last_debounce_time > DEBOUNCE_DELAY){
        if (reading != button_state){
            button_state = reading;
        }
    }

    last_button_state = reading;

    if (_mode == INPUT_PULLUP){
        return !button_state;
    }
    return button_state;

   
}
