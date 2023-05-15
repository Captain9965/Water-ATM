#pragma once
#include "ui/ui.h"

#define SPLASH_SCREEN_DELAY 2000
/* nestle splash screen logo*/
class splashScreenPage: public Page{
    public:
        splashScreenPage();
        int load() override;
        int update() override;
        static splashScreenPage* get_default_instance();
    private:
        long long screen_boot_timer = millis();
};
