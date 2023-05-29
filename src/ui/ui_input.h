#include "common/common.h"
#include "sensors/input/interrupt_button.h"

typedef enum input_flags{
    JOYSTICK_PRESS = 1 << 0,
    BUTTON1_PRESS = 1 << 1,
    JOYSTICK_FLAG_UP = 1 << 2, 
    JOYSTICK_FLAG_DOWN = 1 << 3, 
    JOYSTICK_FLAG_LEFT = 1 << 4, 
    JOYSTICK_FLAG_RIGHT = 1 << 5,
}input_flags_t;

void set_input_flag(input_flags_t flag);
void clear_input_flag(input_flags_t flag);
void input_flags_init();
bool check_input_flag(input_flags_t flag);
void joystick_ISR();

class uiInput{
    public:
        uiInput();
        void init();
        void enable_joystick_button();
        void disable_joystick_button();
        bool joystick_button_pressed();
        bool joystick_down();
        bool joystick_up();
        bool joystick_left();
        bool joystick_right();
        void update_joystick_state();
        static uiInput * get_default_instance();
    private:
        buttonInterrupt joystick_button = buttonInterrupt(SYSTEM_JOYSTICK_BUTTON, joystick_ISR);
};

