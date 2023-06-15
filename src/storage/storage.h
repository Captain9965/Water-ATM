#pragma once 
#include "common/common.h"
#include "SPI.h"
#include "SD.h"
#include "DatabaseOnSD.h"

class storage{
    public:
        storage();
        ~storage();
        bool init();
        void printStorageContent();
        static storage * get_default_instance();
    private:
        MyTable *settingsTable = nullptr; 
        Sd2Card card;
        SdVolume volume;
        SdFile root;
};