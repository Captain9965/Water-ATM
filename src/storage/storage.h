#pragma once 
#include "common/common.h"
#include "SPI.h"
#include "SD.h"
#include "DatabaseOnSD.h"
#include "EEPROM.h"
#include "string.h"

/* EEPROM flash emulation addresses : */
#define QUANTITY_1_ADDRESS                  0
#define QUANTITY_2_ADDRESS                  4
#define QUANTITY_3_ADDRESS                  8
#define QUANTITY_4_ADDRESS                  12
#define QUANTITY_5_ADDRESS                  16
#define QUANTITY_6_ADDRESS                  20
#define CALIBRATION_1_ADDRESS               24
#define CALIBRATION_2_ADDRESS               28
#define CALIBRATION_3_ADDRESS               32
#define CALIBRATION_4_ADDRESS               36
#define TARIFF_1_ADDRESS                    40
#define TARIFF_2_ADDRESS                    44
#define TARIFF_3_ADDRESS                    48
#define TARIFF_4_ADDRESS                    52
#define FLOW_CALCULATION_TIME_ADDRESS       56

class storage{
    public:
        storage();
        ~storage();
        bool init();
        void printSDCardContent();
        bool readValue(int row, int column, String * str);
        bool writeValue(int row, int column, String str);
        void eeprom_wipe();
        static storage * get_default_instance();
    private:
        MyTable *settingsTable = nullptr; 
        Sd2Card card;
        SdVolume volume;
        SdFile root;
};