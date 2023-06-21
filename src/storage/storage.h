#pragma once 
#include "common/common.h"
#include "SPI.h"
#include "SD.h"
#include "DatabaseOnSD.h"
#include "EEPROM.h"
#include "string.h"

/* EEPROM flash emulation addresses : */
#define TARIFF_ADDRESS                      0
#define CALIBRATION_ADDRESS                 3
#define FLOW_CALCULATION_ADDRESS            7
#define TAP1_QUANTITY_1_ADDRESS             11
#define TAP1_QUANTITY_2_ADDRESS             15
#define TAP1_QUANTITY_3_ADDRESS             19
#define TAP1_QUANTITY_4_ADDRESS             23
#define TAP1_QUANTITY_5_ADDRESS             27
#define TAP1_QUANTITY_6_ADDRESS             31

class storage{
    public:
        storage();
        ~storage();
        bool init();
        void printSDCardContent();
        bool readValue(int row, int column, String * str);
        bool writeValue(int row, int column, String str);
        static storage * get_default_instance();
    private:
        MyTable *settingsTable = nullptr; 
        Sd2Card card;
        SdVolume volume;
        SdFile root;
};