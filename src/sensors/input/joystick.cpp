#include "joystick.h"

joystick::joystick(){};

joystick::~joystick(){};

void joystick::init(){
    pinMode(SYSTEM_JOYSTICK_X, INPUT);
    pinMode(SYSTEM_JOYSTICK_Y, INPUT);
    return;
}

int joystick::get_x_value(int & x_value){
    x_reading = analogRead(SYSTEM_JOYSTICK_X);
    /* handle erroneous values here : */
    x_value = x_reading;
    /* return error value: */
    return 0;
}

int joystick::get_y_value(int & y_value){
    y_reading = analogRead(SYSTEM_JOYSTICK_Y);

    y_value = y_reading;
    return 0;
}