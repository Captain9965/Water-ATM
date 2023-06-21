#include "vmc_data.h"


AdminCash * AdminCash::get_default_instance(){
    static AdminCash _instance = AdminCash();
    return &_instance;
}

vmc_data_error_t AdminCash::set(uint32_t value){
    /* data validation: */
    if (!storage::get_default_instance()->writeValue(1, 1, String(value))){
        _value = value;
        _data_error = VMC_DATA_SET_ERROR;
        return _data_error;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

uint32_t AdminCash::DEFAULT_ADMIN_CASH = 0;

bool AdminCash::load(){
    String value;
    if (!storage::get_default_instance()->readValue(1, 1, &value)){
        _value = DEFAULT_ADMIN_CASH;
        _data_error = VMC_DATA_UNSET;
        return false;
    }

    int intValue = atoi(value.c_str());
    _value = (uint32_t)intValue;
    return true;
}



double tariff::DEFAULT_TARIFF = 10.0;

tariff * tariff::get_default_instance(){
    static tariff _instance = tariff();
    return &_instance;
}

vmc_data_error_t tariff::set(double value){
 /* data validation: */
    EEPROM.put(TARIFF_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool tariff::load(){
    double value = 0.00;
    EEPROM.get(TARIFF_ADDRESS, value);
    /* data validation: */
    if (value <= 0.00){
        _value = DEFAULT_TARIFF;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(TARIFF_ADDRESS, DEFAULT_TARIFF);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}


double Calibration::DEFAULT_CALIBRATION = 10.0;

Calibration * Calibration::get_default_instance(){
    static Calibration _instance = Calibration();
    return &_instance;
}

vmc_data_error_t Calibration::set(double value){
 /* data validation: */
    EEPROM.put(CALIBRATION_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool Calibration::load(){
    double value = 0.00;
    EEPROM.get(CALIBRATION_ADDRESS, value);
    /* data validation: */
    if (value <= 0.00){
        _value = DEFAULT_CALIBRATION;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(CALIBRATION_ADDRESS, DEFAULT_CALIBRATION);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}





