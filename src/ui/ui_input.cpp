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

void tap_1_ISR(){
    set_input_flag(TAP_1_BUTTON);
}

void tap_2_ISR(){
    set_input_flag(TAP_2_BUTTON);
}

void tap_3_ISR(){
    set_input_flag(TAP_3_BUTTON);
}

void tap_4_ISR(){
    set_input_flag(TAP_4_BUTTON);
}

void quantity_1_ISR(){
    set_input_flag(QUANTITY_1_BUTTON);
}

void quantity_2_ISR(){
    set_input_flag(QUANTITY_2_BUTTON);
}

void quantity_3_ISR(){
    set_input_flag(QUANTITY_3_BUTTON);
}

void quantity_4_ISR(){
    set_input_flag(QUANTITY_4_BUTTON);
}

void quantity_5_ISR(){
    set_input_flag(QUANTITY_5_BUTTON);
}

void quantity_6_ISR(){
    set_input_flag(QUANTITY_6_BUTTON);
}


uiInput::uiInput(){};

void uiInput::init(){};

uiInput * uiInput::get_default_instance(){
    static uiInput instance;
    return &instance;
}


void uiInput::enable_tap_buttons(){
    tap_1_button.enable();
    tap_2_button.enable();
    tap_3_button.enable();
    tap_4_button.enable();
    clear_input_flag(TAP_1_BUTTON);
    clear_input_flag(TAP_2_BUTTON);
    clear_input_flag(TAP_3_BUTTON);
    clear_input_flag(TAP_4_BUTTON);
}

void uiInput::disable_tap_buttons(){
    tap_1_button.disable();
    tap_2_button.disable();
    tap_3_button.disable();
    tap_4_button.disable();
}


bool uiInput::tap_button_pressed(input_flags_t & tap){
    if(check_input_flag(TAP_1_BUTTON)){
        tap = TAP_1_BUTTON;
        clear_input_flag(TAP_1_BUTTON);
        return true;
    } else if (check_input_flag(TAP_2_BUTTON)){
        tap = TAP_2_BUTTON;
        clear_input_flag(TAP_2_BUTTON);
        return true;
    }else if (check_input_flag(TAP_3_BUTTON)){
        tap = TAP_3_BUTTON;
        clear_input_flag(TAP_3_BUTTON);
        return true;
    }else if (check_input_flag(TAP_4_BUTTON)){
        tap = TAP_4_BUTTON;
        clear_input_flag(TAP_4_BUTTON);
        return true;
    }
    return false;
}

void uiInput::update_push_button_states(){
   
    if(tap_1_button.is_pressed()){
        set_input_flag(TAP_1_BUTTON);
    }
    if(tap_2_button.is_pressed()){
        set_input_flag(TAP_2_BUTTON);
    }
    if(tap_3_button.is_pressed()){
        set_input_flag(TAP_3_BUTTON);
    }
    if(tap_4_button.is_pressed()){
        set_input_flag(TAP_4_BUTTON);
    }
}