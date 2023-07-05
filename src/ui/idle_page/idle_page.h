#pragma once
#include "ui/ui.h"

#define IDLE_NET_CHECK_TIMER_INTERVAL 1000  /* ms*/


class idlePage: public Page{
    public:
        idlePage();
        int load() override;
        int update() override;
        int stop();
        static idlePage* get_default_instance();
    private:
        long long net_check_timer = millis();
};