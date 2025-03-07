#include "storage.h"


storage::storage(){
}

storage::~storage(){

}

storage * storage::get_default_instance(){
    static storage SD_store;
    return & SD_store;
}

void storage::printSDCardContent(){
    assert(settingsTable);
    settingsTable->printTable();
}

bool storage::init(){

    if(!settingsTable){

        /* 
            the table name should not be longer than 12 charactes
            because bad things happen..
        */
        settingsTable = new MyTable("A_cash.csv");
        if (settingsTable->tableExists()){
            DEBUG_INFO_LN("Table exists");
        } else {
            DEBUG_INFO_LN("Table does not exist");
            return false;
        }
        settingsTable->printSDstatus();
        if (!settingsTable->begun()){
            return false;
        }
        
        /* 2 rows and 2 columns: */
        
        /* settings map
            0       1
        0   Key     value
        1   adminCash  00  
        2   adminCard  80 D1 BD 2B
        3   serviceCard F3 79 B3 18
        */
       
    }
    if (settingsTable->countRows() == 0){
        DEBUG_INFO_LN("Initializing table");
        settingsTable->emptyTable();
        settingsTable->begin(2, 2);
        DEBUG_INFO_LN("Writing Default values....");
        settingsTable->writeCell(0, 0, "KEY"); settingsTable->writeCell(0, 1, "VALUE");
        settingsTable->writeCell(1, 0, "adminCash"); settingsTable->writeCell(1, 1, "00");
        settingsTable->writeCell(2, 0, "adminCard"); settingsTable->writeCell(2, 1, "80 D1 BD 2B");
        settingsTable->writeCell(3, 0, "serviceCard"); settingsTable->writeCell(3, 1, "F3 79 B3 18");
    }

    if (settingsTable->countRows() == 0){
        return false;
    }
    // eeprom = new ZEeprom();
    // // Wire.begin();
    // eeprom->begin(Wire,AT24Cxx_BASE_ADDR,AT24C64);
    // const byte address = 0;
    // const byte count = 94;

    // // Declare byte arrays.
    // byte inputBytes[count] = { 0 };
    // byte outputBytes[count] = { 0 };

    // // Fill input array with printable characters. See ASCII table for more
    // // details.
    // for (byte i = 0; i < count; i++)
    // {    
    //     inputBytes[i] = i + 33;
    // }

    // // Write input array to EEPROM memory.
    // DEBUG_INFO_LN("Write bytes to EEPROM memory...");
    // eeprom->writeBytes(address, count, inputBytes);

    // // Read array with bytes read from EEPROM memory.
    // DEBUG_INFO_LN("Read bytes from EEPROM memory...");
    // eeprom->readBytes(address, count, outputBytes);
    
    // // Print read bytes.
    // DEBUG_INFO_LN("Read bytes:");
    // for (byte i = 0; i < count; i++)
    // {
    //     DEBUG_INFO_LN(outputBytes[i]);
    //     DEBUG_INFO_LN(" ");
    // }
    // DEBUG_INFO_LN("");
    return true;
}

bool storage::readValue(int row , int column, String * str){
    if (!settingsTable){
        return false;
    }
    if (!settingsTable->begun()){
        return false;
    }
    *str = settingsTable->readCell(row,column);
    if (str->length() < 1){
        return false;
    }
    return true;
}

bool storage::writeValue(int row, int column , String str){
    if (!settingsTable){
        return false;
    }
    if (!settingsTable->begun()){
        return false;
    }
    return settingsTable->writeCell(row, column, str);
}

void storage::eeprom_wipe(){
    for (int i = 0; i < EEPROM.length(); i++){
        EEPROM.write(i, 0);
    }
}

void storage::write_eeprom(int device_address, unsigned int write_address, byte data){
    Wire.beginTransmission(device_address);
    Wire.write((int)(write_address >> 8));   // MSB
    Wire.write((int)(write_address & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();
}

byte storage::read_eeprom(int device_address, unsigned int read_address){
    byte rdata = 0xFF;
    Wire.beginTransmission(device_address);
    Wire.write((int)(read_address >> 8));   // MSB
    Wire.write((int)(read_address & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(device_address,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
}
