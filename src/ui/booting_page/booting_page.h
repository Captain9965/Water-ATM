#pragma once
#include "ui/ui.h"

#define BOOT_NET_CHECK_TIMER_INTERVAL 1000  /* ms*/

/* nestle splash screen logo*/
class bootingPage: public Page{
    public:
        bootingPage();
        int load() override;
        int update() override;
        static bootingPage* get_default_instance();
    private:
        long long screen_boot_timer = millis();
        long long net_check_timer = millis();
};