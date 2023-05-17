#pragma once 
#include "common/common.h"
#include "STM32RTC.h"

class systemTime{
    public:
        systemTime();
        ~systemTime();
        void init();
        void setTime();
        void setDate();
        void getTime();
        void getDate();
    private:
        STM32RTC & rtc;

};