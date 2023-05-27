#pragma once
#include "common/common.h"

typedef enum joystick_state{
    JOYSTICK_UP,
    JOYSTICK_DOWN, 
    JOYSTICK_LEFT, 
    JOYSTICK_RIGHT,
    JOYSTICK_NEUTRAL,
    JOYSTICK_ERROR
}joystick_state_t;

class joystick{
    public:
        joystick();
        ~joystick();
        void init();
        int read();
        int get_y_value();
        int get_x_value();
        bool is_right();
        bool is_left();
        bool is_up();
        bool is_down();
        joystick_state_t get_state();
        static joystick * get_default_instance();
    private:
        int x_reading = 0;
        int y_reading = 0;
        int button_state = 0;
};