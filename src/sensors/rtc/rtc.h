#pragma once 
#include "common/common.h"
#include "STM32RTC.h"

typedef struct time{
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
} system_time_t;

class systemTime{
    public:
        systemTime();
        ~systemTime();
        void init();
        void setTime(system_time_t & time);
        // void setDate();
        void getTime( system_time_t & time );
        // void getDate();
        static systemTime * get_default_instance();
    private:
        STM32RTC & rtc;

};