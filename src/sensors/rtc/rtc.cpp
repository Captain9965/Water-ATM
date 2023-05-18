#include "rtc.h"

systemTime::systemTime():rtc(STM32RTC::getInstance()){

}

systemTime * systemTime::get_default_instance(){
    static systemTime instance;
    return & instance;
}

systemTime::~systemTime(){};

void systemTime::init(){
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    /**
     * @params:
     * resetTime – if true reconfigures the RTC
     *  format – hour format: HOUR_12 or HOUR_24(default)
     * 
     */
    rtc.begin(false);
    return;
}

void systemTime::getTime(system_time_t & time ){
    time.day = rtc.getDay();
    time.month = rtc.getMonth();
    time.year = rtc.getYear();
    time.hour = rtc.getHours();
    time.minutes = rtc.getMinutes();
    time.seconds = rtc.getSeconds();
    return;
}

void systemTime::setTime(system_time_t & time){
    rtc.setDay(time.day);
    rtc.setMonth(time.month);
    rtc.setYear(time.year);
    rtc.setHours(time.hour);
    rtc.setMinutes(time.minutes);
    rtc.setSeconds(time.seconds);
    return;
}