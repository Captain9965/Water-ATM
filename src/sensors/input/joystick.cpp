#include "joystick.h"

joystick::joystick(){};

joystick::~joystick(){};

void joystick::init(){
    pinMode(SYSTEM_JOYSTICK_X, INPUT);
    pinMode(SYSTEM_JOYSTICK_Y, INPUT);
    return;
}

int joystick::read(){
    int x_value = analogRead(SYSTEM_JOYSTICK_X);
    int y_value = analogRead(SYSTEM_JOYSTICK_Y);
    /* handle erroneous values here, default value is zero incase of errors : */
    x_reading = x_value;
    y_reading = y_value;
    /* return error value: */   
    return 0;
}

int joystick::get_x_value(){

    return x_reading;
}

int joystick::get_y_value(){

    return y_reading;
}

bool joystick::is_right(){
    
    if ( x_reading >= 1000 && y_reading  >= 500){
        return true;
    }
    return false;

}

bool joystick::is_left(){
    if ( x_reading == 0 && y_reading >= 350){
        return true;
    }
    return false;
}

bool joystick::is_up(){
    if ( x_reading >= 250 && y_reading >= 800 ){
        return true;
    }
    return false;
}

bool joystick::is_down(){
    if ( x_reading >=  160 && y_reading == 0){
        return true;
    }
    return false;
}

joystick_state_t joystick::get_state(){
    if(read() < 0){
        return JOYSTICK_ERROR;
    }
    if(is_right()){
        return JOYSTICK_RIGHT;
    } else if(is_left()){
        return JOYSTICK_LEFT;
    } else if(is_up()){
        return JOYSTICK_UP;
    } else if(is_down()){
        return JOYSTICK_DOWN;
        /* should the button press override all other inputs??*/
    }
    return JOYSTICK_ERROR;
}

joystick * joystick::get_default_instance(){
    static joystick instance;
    return &instance;
}