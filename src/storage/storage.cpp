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