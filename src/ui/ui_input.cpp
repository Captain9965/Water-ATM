#include "ui_input.h"

static uint32_t INPUT_FLAGS = 0;

void set_input_flag(input_flags_t flag){
    INPUT_FLAGS |= flag;
    return;
}

void clear_input_flag(input_flags_t flag){
    INPUT_FLAGS &= ~(flag);
    return;
}

void input_flags_init(){
    INPUT_FLAGS = 0;
    return;
}

bool check_input_flag(input_flags_t flag){
    return (INPUT_FLAGS & flag);
}

void joystick_ISR(){
    set_input_flag(JOYSTICK_PRESS);
}


uiInput::uiInput(){};

void uiInput::init(){};

uiInput * uiInput::get_default_instance(){
    static uiInput instance;
    return &instance;
}

void uiInput::enable_joystick_button(){
    joystick_button.enable();
}

void uiInput::disable_joystick_button(){
    joystick_button.disable();
}

bool uiInput::joystick_button_pressed(){
    if(check_input_flag(JOYSTICK_PRESS)){
        clear_input_flag(JOYSTICK_PRESS);
        return true;
    }
    return false;
}