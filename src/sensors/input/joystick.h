#pragma once
#include "common/common.h"



class joystick{
    public:
        joystick();
        ~joystick();
        void init();
        int get_y_value(int & y_value);
        int get_x_value(int & x_value);
        bool is_right();
        bool is_left();
        bool is_up();
        bool is_down();
        static joystick * get_default_instance();
    private:
        int x_reading = 0;
        int y_reading = 0;
};