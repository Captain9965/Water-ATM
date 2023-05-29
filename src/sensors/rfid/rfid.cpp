#include "rfid.h"


RFID * RFID::get_default_instance(){
    static RFID rfid_reader = RFID(SYSTEM_RFID_SS_PIN, SYSTEM_RFID_RESET_PIN);
    return &rfid_reader;
}

RFID::RFID(uint32_t cs_pin, uint32_t reset_pin){
    _instance = new MFRC522(cs_pin, reset_pin);
    assert(_instance);
    return;
}

RFID::~RFID(){
    delete _instance;
    _instance = nullptr;
    return;
}

bool RFID::init(){
    SPI.begin();
    _instance->PCD_Init();
    return true;

}

String RFID::read_uid(){
    if (!_instance->PICC_IsNewCardPresent() || !_instance->PICC_ReadCardSerial()) return String("");
    String uid = "";
    for (byte i = 0 ; i < _instance->uid.size; i ++){
        uid.concat(String(_instance->uid.uidByte[i] < 0x10 ? " 0" : " "));
        uid.concat(String(_instance->uid.uidByte[i], HEX));
    }
    uid.toUpperCase();
    return uid;
}


