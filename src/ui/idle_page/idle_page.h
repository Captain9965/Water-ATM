#pragma once
#include "ui/ui.h"
#include "vmc/vmc_flags.h"

#define IDLE_NET_CHECK_TIMER_INTERVAL 1000  /* ms*/


class idlePage: public Page{
    public:
        idlePage();
        int load() override;
        int update() override;
        int stop(vmc_flags_t tap);
        static idlePage* get_default_instance();
    private:
        long long net_check_timer = 0;
        bool _clear_display = true;
};