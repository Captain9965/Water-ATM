#include "polling_button.h"

pollingButton::pollingButton(uint32_t pin, uint8_t mode):_pin(pin), _mode(mode){
    pinMode(_pin, _mode);
    if (_mode == INPUT_PULLUP){
        last_button_state = HIGH;
        button_state = HIGH;
    } else{
        last_button_state = LOW;
        button_state = LOW;
    }
    return;
};

bool pollingButton::is_pressed(){
    switch (reading_state)
    {
    case BUTTON_READING:
        if(get_button_state() == HIGH){
            reading_state = BUTTON_WAITING_RELEASE;
        }
        break;
    case BUTTON_WAITING_RELEASE:
        if(get_button_state() == LOW){
            reading_state = BUTTON_READING;
            return true;
        }
        break;
    default:
        break;
    }
    return false;

   
}
int pollingButton::get_button_state(){
    
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
        because when the button is disabled and later reenabled, it causes
        the button to default to the last button state which will cause false press detection
    
    */
    if(!enabled){
        return 0;
    }

    if (_mode == INPUT_PULLUP){
        return !button_state;
    }
    return button_state;

}