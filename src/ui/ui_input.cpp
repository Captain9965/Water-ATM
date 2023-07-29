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

void uiInput::init(){
    disable_quantity_buttons();
};

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

void uiInput::enable_quantity_buttons(){
    quantity_1_button.enable();
    quantity_2_button.enable();
    quantity_3_button.enable();
    quantity_4_button.enable();
    quantity_5_button.enable();
    quantity_6_button.enable();
}

void uiInput::disable_quantity_buttons(){
    
    quantity_1_button.disable();
    quantity_2_button.disable();
    quantity_3_button.disable();
    quantity_4_button.disable();
    quantity_5_button.disable();
    quantity_6_button.disable();
}

void uiInput::enable_tap_buttons(){
    tap_1_button.enable();
    tap_2_button.enable();
    tap_3_button.enable();
    tap_4_button.enable();
}

void uiInput::disable_tap_buttons(){
    tap_1_button.disable();
    tap_2_button.disable();
    tap_3_button.disable();
    tap_4_button.disable();
}

bool uiInput::joystick_button_pressed(){
    if(check_input_flag(JOYSTICK_PRESS)){
        clear_input_flag(JOYSTICK_PRESS);
        return true;
    }
    return false;
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

bool uiInput::quantity_button_pressed(input_flags_t &quantity){
    bool ret = false;
    if(check_input_flag(QUANTITY_1_BUTTON)){
        quantity = QUANTITY_1_BUTTON;
        clear_input_flag(QUANTITY_1_BUTTON);
        ret = true;
    } else if (check_input_flag(QUANTITY_2_BUTTON)){
        quantity = QUANTITY_2_BUTTON;
        clear_input_flag(QUANTITY_2_BUTTON);
        ret = true;
    } else if (check_input_flag(QUANTITY_3_BUTTON)){
        quantity = QUANTITY_3_BUTTON;
        clear_input_flag(QUANTITY_3_BUTTON);
        ret = true;
    } else if (check_input_flag(QUANTITY_4_BUTTON)){
        quantity = QUANTITY_4_BUTTON;
        clear_input_flag(QUANTITY_4_BUTTON);
        ret = true;
    } else if (check_input_flag(QUANTITY_5_BUTTON)){
        quantity = QUANTITY_5_BUTTON;
        clear_input_flag(QUANTITY_5_BUTTON);
        ret = true;
    } else if (check_input_flag(QUANTITY_6_BUTTON)){
        quantity = QUANTITY_6_BUTTON;
        clear_input_flag(QUANTITY_6_BUTTON);
        ret = true;
    }
    return ret;
}

bool uiInput::door_closed(){
    if(check_input_flag(DOOR_CLOSED)){
        clear_input_flag(DOOR_CLOSED);
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

void uiInput::update_push_button_states(){
    if (joystick_button.is_pressed()){
        set_input_flag(JOYSTICK_PRESS);
    }
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
    if(quantity_1_button.is_pressed()){
        set_input_flag(QUANTITY_1_BUTTON);
    }
    if(quantity_2_button.is_pressed()){
        set_input_flag(QUANTITY_2_BUTTON);
    }
    if(quantity_3_button.is_pressed()){
        set_input_flag(QUANTITY_3_BUTTON);
    }
    if(quantity_4_button.is_pressed()){
        set_input_flag(QUANTITY_4_BUTTON);
    }
    if(quantity_5_button.is_pressed()){
        set_input_flag(QUANTITY_5_BUTTON);
    }
    if(quantity_6_button.is_pressed()){
        set_input_flag(QUANTITY_6_BUTTON);
    }
    if(door_switch.is_pressed()){
        set_input_flag(DOOR_CLOSED);
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