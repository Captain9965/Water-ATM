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
        settingsTable = new MyTable("machine_settings.csv");
        settingsTable->printSDstatus();
        if (!settingsTable->begun()){
            return false;
        }
        /* 7 rows and 2 columns: */
        settingsTable->begin(7, 2);
        /* settings map
            0       1
        0   Key     value
        1   Tap1q1  20
        2   Tap1q2  30
        3   Tap1q3  40
        4   Tap1q4  50
        5   Tap1q5  60
        6   Tap1q6  70   
        */
       
    }
    if (settingsTable->countCols() == 0 && settingsTable->countRows() == 0){
        DEBUG_INFO_LN("Writing Default values....");
        settingsTable->writeCell(0, 0, "KEY"); settingsTable->writeCell(0, 1, "VALUE");
        settingsTable->writeCell(1, 0, "Tap1q1"); settingsTable->writeCell(1, 1, "20");
        settingsTable->writeCell(2, 0, "Tap1q2"); settingsTable->writeCell(2, 1, "30");
        settingsTable->writeCell(3, 0, "Tap1q3"); settingsTable->writeCell(3, 1, "40");
        settingsTable->writeCell(4, 0, "Tap1q4"); settingsTable->writeCell(4, 1, "50");
        settingsTable->writeCell(5, 0, "Tap1q5"); settingsTable->writeCell(5, 1, "60");
        settingsTable->writeCell(6, 0, "Tap1q6"); settingsTable->writeCell(6, 1, "70");
    }
    if (settingsTable->countCols() == 0 && settingsTable->countRows() == 0){
        return false;
    }
    return true;
    
}