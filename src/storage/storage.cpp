#include "storage.h"


storage::storage(){


}

storage::~storage(){

}

storage * storage::get_default_instance(){
    static storage SD_store;
    return & SD_store;
}

bool storage::init(){
    if (!SD.begin(SYSTEM_SD_CS_PIN)){
        return false;
    }
    return true;
}