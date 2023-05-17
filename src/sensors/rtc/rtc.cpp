#include "rtc.h"

systemTime::systemTime():rtc(STM32RTC::getInstance()){

}

systemTime::~systemTime(){};

void systemTime::init(){
    rtc.setClockSource()
}