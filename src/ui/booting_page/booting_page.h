#pragma once
#include "ui/ui.h"


/* booting page: */
class bootingPage: public Page{
    public:
        bootingPage();
        int load() override;
        int update() override;
        int stop();
        static bootingPage* get_default_instance();
    private:
        long long screen_boot_timer = millis();
};