#include "common/common.h"
#include "sensors/input/interrupt_button.h"
#include "sensors/input/polling_button.h"

typedef enum input_flags{
    JOYSTICK_PRESS = 1 << 0,
    JOYSTICK_FLAG_UP = 1 << 1, 
    JOYSTICK_FLAG_DOWN = 1 << 2, 
    JOYSTICK_FLAG_LEFT = 1 << 3, 
    JOYSTICK_FLAG_RIGHT = 1 << 4,
    TAP_1_BUTTON = 1 << 5, 
    TAP_2_BUTTON = 1 << 6,
    TAP_3_BUTTON = 1 << 7,
    TAP_4_BUTTON = 1 << 8,
    QUANTITY_1_BUTTON = 1 << 9,
    QUANTITY_2_BUTTON = 1 << 10,
    QUANTITY_3_BUTTON = 1 << 11,
    QUANTITY_4_BUTTON = 1 << 12,
    QUANTITY_5_BUTTON = 1 << 13,
    QUANTITY_6_BUTTON = 1 << 14,
    DOOR_CLOSED = 1 << 15,
    JOYSTICK_LONG_PRESS = 1 << 16
}input_flags_t;

void set_input_flag(input_flags_t flag);
void clear_input_flag(input_flags_t flag);
void input_flags_init();
bool check_input_flag(input_flags_t flag);
void joystick_ISR();
void tap_1_ISR();
void tap_2_ISR();
void tap_3_ISR();
void tap_4_ISR();
void quantity_1_ISR();
void quantity_2_ISR();
void quantity_3_ISR();
void quantity_4_ISR();
void quantity_5_ISR();
void quantity_6_ISR();

class uiInput{
    public:
        uiInput();
        void init();
        void enable_joystick_button();
        void enable_tap_buttons();
        void disable_tap_buttons();
        void enable_quantity_buttons();
        void disable_quantity_buttons();
        void disable_joystick_button();
        bool joystick_button_pressed();
        bool joystick_button_long_pressed();
        bool tap_button_pressed(input_flags_t & tap);
        bool quantity_button_pressed(input_flags_t & quantity);
        bool door_closed();
        bool joystick_down();
        bool joystick_up();
        bool joystick_left();
        bool joystick_right();
        void update_joystick_state();
        void update_push_button_states();
        static uiInput * get_default_instance();
    private:
        pollingButton joystick_button = pollingButton(SYSTEM_JOYSTICK_BUTTON,INPUT_PULLUP);
        pollingButton tap_1_button = pollingButton(SYSTEM_TAP_1, INPUT_PULLDOWN);
        pollingButton tap_2_button = pollingButton(SYSTEM_TAP_2, INPUT_PULLDOWN);
        pollingButton tap_3_button = pollingButton(SYSTEM_TAP_3, INPUT_PULLDOWN);
        pollingButton tap_4_button = pollingButton(SYSTEM_TAP_4, INPUT_PULLDOWN);
        pollingButton quantity_1_button = pollingButton(SYSTEM_QUANTITY_1, INPUT_PULLDOWN);
        pollingButton quantity_2_button = pollingButton(SYSTEM_QUANTITY_2, INPUT_PULLDOWN);
        pollingButton quantity_3_button = pollingButton(SYSTEM_QUANTITY_3, INPUT_PULLDOWN);
        pollingButton quantity_4_button = pollingButton(SYSTEM_QUANTITY_4, INPUT_PULLDOWN);
        pollingButton quantity_5_button = pollingButton(SYSTEM_QUANTITY_5, INPUT_PULLDOWN);
        pollingButton quantity_6_button = pollingButton(SYSTEM_QUANTITY_6, INPUT_PULLDOWN);
        pollingButton door_switch = pollingButton(SYSTEM_DOORSWITCH, INPUT_PULLDOWN);
};

