#pragma once
#include "ui/ui.h"

class dispensingPage: public Page{
    public:
        dispensingPage();
        int load() override;
        int update() override;
        static dispensingPage* get_default_instance();
    private:
        void display_button_pressed_for_dispense();
};