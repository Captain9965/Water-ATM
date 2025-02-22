#pragma once
#include "ui/ui.h"
#include "vmc/vmc_flags.h"
#include "services/dispense_system.h"


typedef enum ui_state{
    UI_IDLE,
    UI_QUANTITY_SELECT,
    UI_AWAIT_QUANTITY_SELECTION,
    UI_SHOW_PAYMENT_DUE,
    UI_AWAIT_PAYMENT,
    UI_DISPENSING_RUNNING,
    UI_DISPENSING_STARTING,
    UI_NOP,
    UI_CANCELLED,
    UI_DISPENSING_LOW_FLOW_RATE
}ui_state_t;

class dispensingPage: public Page{
    public:
        dispensingPage(vmc_flags_t initialTap);
        int load() override;
        int update() override;
        int stop();
        static dispensingPage* get_default_instance(vmc_flags_t initialTap);
    private:
        void clear_amount();
        void check_for_quantity_selection();
        void check_for_tap_selection();
        void update_dispense_quantities();
        void display_button_pressed_for_dispense(vmc_flags_t & tap);
        ui_state_t switch_ui_state();
        vmc_flags_t  _initialTap;
        dispenseGroup * _dispense_group = nullptr;
        DispenseSystem * _current_dispense_instance = nullptr;
        float amount1, amount2, amount3, amount4;
        ui_state_t ui_state;
};