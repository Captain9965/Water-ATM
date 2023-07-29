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

    /* This condition check for whether the button is enabled or not had better be here
        because when the button is disable and later reenabled, it causes
        the button to default to the last button state which will cause false press detection
    
    */
    if(!enabled){
        return false;
    }

    if (_mode == INPUT_PULLUP){
        return !button_state;
    }
    return button_state;

   
}
