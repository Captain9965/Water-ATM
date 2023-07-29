#pragma once
#include "ui/ui.h"
#include "vmc/vmc_flags.h"
#include "services/dispense_system.h"

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
        void update_dispense_quantities();
        void display_button_pressed_for_dispense(vmc_flags_t & tap);
        vmc_flags_t  _initialTap;
        DispenseSystem * _dispense_instance = nullptr;
        float amount1, amount2, amount3, amount4;
};