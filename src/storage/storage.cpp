#include "storage.h"


storage::storage(){


}

storage::~storage(){

}

storage * storage::get_default_instance(){
    static storage SD_store;
    return & SD_store;
}

void storage::printStorageContent(){
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
        */
       
    }
    if (settingsTable->countRows() == 0){
        DEBUG_INFO_LN("Initializing table");
        settingsTable->emptyTable();
        settingsTable->begin(2, 2);
        DEBUG_INFO_LN("Writing Default values....");
        settingsTable->writeCell(0, 0, "KEY"); settingsTable->writeCell(0, 1, "VALUE");
        settingsTable->writeCell(1, 0, "adminCash"); settingsTable->writeCell(1, 1, "00");
    }

    if (settingsTable->countRows() == 0){
        return false;
    }
    return true;
    
}