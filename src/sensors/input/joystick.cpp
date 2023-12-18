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
    if ( x_reading >= 0 && y_reading >= 800 ){
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
        waiting_state = JOYSTICK_RIGHT;
        return JOYSTICK_WAIT_RELEASE;
    }
    else if(is_left()){
        waiting_state = JOYSTICK_LEFT;
        return JOYSTICK_WAIT_RELEASE;
    } 
    else if(is_up()){
        return JOYSTICK_UP;
    } else if(is_down()){
        return JOYSTICK_DOWN;
    }
    else{
        if(waiting_state != JOYSTICK_NEUTRAL){
            joystick_state_t return_state = waiting_state;
            waiting_state = JOYSTICK_NEUTRAL;
            return return_state;
        }
        else{
            waiting_state = JOYSTICK_NEUTRAL;
            return JOYSTICK_NEUTRAL;
        }
        waiting_state = JOYSTICK_NEUTRAL;
        return JOYSTICK_NEUTRAL;
    }
}

joystick * joystick::get_default_instance(){
    static joystick instance;
    return &instance;
}