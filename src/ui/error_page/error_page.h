#pragma once
#include "ui/ui.h"


/* error page*/
class errorPage: public Page{
    public:
        errorPage(const char * error_msg);
        int load() override;
        int update() override;
        static errorPage* get_default_instance(const char * error_msg);
    private:
        long long screen_boot_timer = millis();
        const char * _error_message = nullptr;
};