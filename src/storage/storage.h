#pragma once 
#include "common/common.h"
#include "SPI.h"
#include "SD.h"

class storage{
    public:
        storage();
        ~storage();
        bool init();
        static storage * get_default_instance();
    private:
        Sd2Card card;
        SdVolume volume;
        SdFile root;
};