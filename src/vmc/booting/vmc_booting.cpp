#include "vmc_booting.h"
#include "../idle/vmc_idle.h"
#include "../ota/vmc_ota.h"
#include "../vmc_flags.h"
#include "sensors/rtc/rtc.h"
#include "storage/storage.h"
#include "vmc/vmc_data.h"
#include "ota/ota.h"

vmc_booting::vmc_booting(){
    id = "BOOTING";
}

int vmc_booting::start(){
    started = true;
    DEBUG_INFO_LN("booting state starting..");

    vmc_flags_init();

    _error = init_comms();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Comms failed to initialize");
        set_vmc_flag(VMC_MEMORY_ERROR);
    }

    return 0;
}

int vmc_booting::run(){
    if(!started){
        start();
    }

    _error = init_sensors();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Sensors init fail");
    }

    _error = init_storage();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Storage failed to initialize");
        set_vmc_flag(VMC_STORAGE_ERROR);
    }

    _error = init_settings();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Settings init fail");
    }

    _error = init_actuators();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Actuators init fail");
    }

    _error = init_services();
    if (_error != VMC_OK){
        DEBUG_INFO_LN("Services init fail");
    }

    stop();
    return 0;
}

int vmc_booting::stop(){
    DEBUG_INFO_LN("booting state stopping...");
    set_vmc_flag(VMC_BOOTING_DONE);

    if (check_for_ota_update()) {
        this->vmc->set_state(vmc_ota::get_default_instance());
    } else {
        this->vmc->set_state(vmc_idle::get_default_instance());
    }
    return 0;
}

bool vmc_booting::check_for_ota_update(){
    OTAManager* ota = OTAManager::get_instance();

    uint32_t fw_size = 0, fw_crc = 0;

    if (ota->detect(&fw_size, &fw_crc)) {
        // firmware.bin found — verify CRC and map sectors
        if (ota->begin(fw_size, fw_crc)) {
            DEBUG_INFO_LN("[OTA] Update ready — entering OTA state");
            return true;
        }
        DEBUG_INFO_LN("[OTA] begin() failed — skipping update");
    }

    return false;
}

vmc_booting* vmc_booting::get_default_instance(){
    static vmc_booting vmc_state;
    return &vmc_state;
}

vmc_error_t vmc_booting::init_comms(){
    DEBUG_INFO_LN("init_comms");
    Comms::get_instance()->init();
    return VMC_OK;
}

vmc_error_t vmc_booting::init_storage(){
    DEBUG_INFO_LN("init_storage");
    if(storage::get_default_instance()->init()){
        return VMC_OK;
    }
    return VMC_ERROR_STORAGE_INIT;
}

vmc_error_t vmc_booting::init_settings(){
    DEBUG_INFO_LN("init_settings");
    vmc_error_t return_val = VMC_OK;

    if(!Tariff::get_default_instance()->load()){
        DEBUG_INFO_LN("Tariff failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }
    if(!Calibration::get_default_instance()->load()){
        DEBUG_INFO_LN("Calibration failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }
    if(!Quantities::get_default_instance()->load()){
        DEBUG_INFO_LN("Quantities failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }
    if(!FlowCalculationTime::get_default_instance()->load()){
        DEBUG_INFO_LN("Flow calculation time failed to load");
        return_val = VMC_ERROR_SETTINGS_INIT;
    }
    return return_val;
}

vmc_error_t vmc_booting::init_sensors(){
    DEBUG_INFO_LN("init_sensors");
    systemTime::get_default_instance()->init();
    return VMC_OK;
}

vmc_error_t vmc_booting::init_actuators(){
    DEBUG_INFO_LN("init_actuators");
    return VMC_OK;
}

vmc_error_t vmc_booting::init_services(){
    return VMC_OK;
}
