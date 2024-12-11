#pragma once
#include "SPI.h"
#include "MFRC522.h"
#include "common/common.h"

#define SERVICE_TAG          "F3 79 B3 18"
#define VENDING_CARD        "13 FC 7B A3"
#define ADMIN_CARD         "80 D1 BD 2B"    

/* will later have an interface for all sensors*/
class RFID{
    public:
        RFID(uint32_t cs_pin, uint32_t reset_pin);
        ~RFID();
        bool init();
        String  read_uid();
        static RFID * get_default_instance();
    private:
        MFRC522 * _instance = nullptr;
};



