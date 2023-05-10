#pragma once
#include "ui/ui.h"

/* nestle splash screen logo*/
class bootingPage: public Page{
    public:
        bootingPage();
        int load() override;
        int update() override;
        static bootingPage* get_default_instance();
    private:
        long long screen_boot_timer = millis();
};