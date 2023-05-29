#include "ui_input.h"
#include "sensors/input/joystick.h"

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

void uiInput::update_joystick_state(){
    joystick_state_t state = joystick::get_default_instance()->get_state();
    switch (state)
    {
    case JOYSTICK_UP:
        set_input_flag(JOYSTICK_FLAG_UP);
        break;
    case JOYSTICK_DOWN:
        set_input_flag(JOYSTICK_FLAG_DOWN);
        break;
    case JOYSTICK_RIGHT:
        set_input_flag(JOYSTICK_FLAG_RIGHT);
        break;
    case JOYSTICK_LEFT:
        set_input_flag(JOYSTICK_FLAG_LEFT);
        break;
    default:
        break;
    }

}
bool uiInput::joystick_up(){
    if(check_input_flag(JOYSTICK_FLAG_UP)){
        clear_input_flag(JOYSTICK_FLAG_UP);
        return true;
    }
    return false;
}

bool uiInput::joystick_down(){
    if(check_input_flag(JOYSTICK_FLAG_DOWN)){
        clear_input_flag(JOYSTICK_FLAG_DOWN);
        return true;
    }
    return false;
}

bool uiInput::joystick_right(){
    if(check_input_flag(JOYSTICK_FLAG_RIGHT)){
        clear_input_flag(JOYSTICK_FLAG_RIGHT);
        return true;
    }
    return false;
}

bool uiInput::joystick_left(){
    if(check_input_flag(JOYSTICK_FLAG_LEFT)){
        clear_input_flag(JOYSTICK_FLAG_LEFT);
        return true;
    }
    return false;
}