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



float tariff::DEFAULT_TARIFF = 10.0;

tariff * tariff::get_default_instance(){
    static tariff _instance = tariff();
    return &_instance;
}

vmc_data_error_t tariff::set(float value){
 /* data validation: */
    EEPROM.put(TARIFF_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool tariff::load(){
    float value = 0.00;
    EEPROM.get(TARIFF_ADDRESS, value);
    /* data validation: */
    if (!(value > 0.00) || isnan(value)){
        _value = DEFAULT_TARIFF;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(TARIFF_ADDRESS, DEFAULT_TARIFF);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}


float Calibration::DEFAULT_CALIBRATION = 10.0;

Calibration * Calibration::get_default_instance(){
    static Calibration _instance = Calibration();
    return &_instance;
}

vmc_data_error_t Calibration::set(float value){
 /* data validation: */
    EEPROM.put(CALIBRATION_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool Calibration::load(){
    float value = 0.00;
    EEPROM.get(CALIBRATION_ADDRESS, value);
    /* data validation: */
    if (!(value > 0.00) || isnan(value)){
        _value = DEFAULT_CALIBRATION;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(CALIBRATION_ADDRESS, DEFAULT_CALIBRATION);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}


float FlowCalculation::DEFAULT_FLOW_CALCULATION = 10.0;

FlowCalculation * FlowCalculation::get_default_instance(){
    static FlowCalculation _instance = FlowCalculation();
    return &_instance;
}

vmc_data_error_t FlowCalculation::set(float value){
 /* data validation: */
    EEPROM.put(FLOW_CALCULATION_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool FlowCalculation::load(){
    float value = 0.00;
    EEPROM.get(FLOW_CALCULATION_ADDRESS, value);
    /* data validation: */
    if (!(value > 0.00) || isnan(value)){
        _value = DEFAULT_FLOW_CALCULATION;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(FLOW_CALCULATION_ADDRESS, DEFAULT_FLOW_CALCULATION);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}

float Quantities::DEFAULT_QUANTITTY_1 = 10.0;
float Quantities::DEFAULT_QUANTITTY_2 = 20.0;
float Quantities::DEFAULT_QUANTITTY_3 = 30.0;
float Quantities::DEFAULT_QUANTITTY_4 = 40.0;
float Quantities::DEFAULT_QUANTITTY_5 = 50.0;
float Quantities::DEFAULT_QUANTITTY_6 = 60.0;

Quantities * Quantities::get_default_instance(){
    static Quantities _instance = Quantities();
    return &_instance;
}

vmc_data_error_t Quantities::set(quantities_t value){
 /* see whether value has changed and if so, change setting: */
 if(value.quantity1 != _value.quantity1){
    /* data validation: */
    DEBUG_INFO_LN("Quantity1 has changed");
    EEPROM.put(QUANTITY_1_ADDRESS, value.quantity1);
    _value.quantity1 = value.quantity1;
    _data_error = VMC_DATA_OK;
 }

 if(value.quantity2 != _value.quantity2){
    /* data validation: */
    DEBUG_INFO_LN("Quantity2 has changed");
    EEPROM.put(QUANTITY_2_ADDRESS, value.quantity2);
    _value.quantity2 = value.quantity2;
    _data_error = VMC_DATA_OK;
 }

 if(value.quantity3 != _value.quantity3){
    /* data validation: */
    DEBUG_INFO_LN("Quantity3 has changed");
    EEPROM.put(QUANTITY_3_ADDRESS, value.quantity3);
    _value.quantity3 = value.quantity3;
    _data_error = VMC_DATA_OK;
 }
 if(value.quantity4 != _value.quantity4){
    /* data validation: */
    DEBUG_INFO_LN("Quantity4 has changed");
    EEPROM.put(QUANTITY_4_ADDRESS, value.quantity4);
    _value.quantity4 = value.quantity4;
    _data_error = VMC_DATA_OK;
 }
 if(value.quantity5 != _value.quantity5){
    /* data validation: */
    DEBUG_INFO_LN("Quantity5 has changed");
    EEPROM.put(QUANTITY_5_ADDRESS, value.quantity5);
    _value.quantity5 = value.quantity5;
    _data_error = VMC_DATA_OK;
 }
 if(value.quantity6 != _value.quantity6){
    /* data validation: */
    DEBUG_INFO_LN("Quantity6 has changed");
    EEPROM.put(QUANTITY_6_ADDRESS, value.quantity6);
    _value.quantity6 = value.quantity6;
    _data_error = VMC_DATA_OK;
 }
 
 return _data_error;
    
}

bool Quantities::load(){
    bool ret = true;
    float value1 = 0.00;
    float value2 = 0.00;
    float value3 = 0.00;
    float value4 = 0.00;
    float value5 = 0.00;
    float value6 = 0.00;

    EEPROM.get(QUANTITY_1_ADDRESS, value1);
    /* data validation: */
    if (!(value1 > 0.00) || isnan(value1)){
        _value.quantity1 = DEFAULT_QUANTITTY_1;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(QUANTITY_1_ADDRESS, 0);
        EEPROM.put(QUANTITY_1_ADDRESS, DEFAULT_QUANTITTY_1);
        ret = false;
    } else{
        _value.quantity1 = value1;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(QUANTITY_2_ADDRESS, value2);
    /* data validation: */
    if (!(value2 > 0.00) || isnan(value2)){
        _value.quantity2 = DEFAULT_QUANTITTY_2;
        _data_error = VMC_DATA_UNSET;
        DEBUG_INFO_LN("Resetting quantitiy 2");
        EEPROM.write(QUANTITY_2_ADDRESS, 0);
        EEPROM.put(QUANTITY_2_ADDRESS, DEFAULT_QUANTITTY_2);
        ret = false;
    } else{
        _value.quantity2 = value2;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(QUANTITY_3_ADDRESS, value3);
    /* data validation: */
    if (!(value3 > 0.00) || isnan(value3)){
        _value.quantity3 = DEFAULT_QUANTITTY_3;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(QUANTITY_3_ADDRESS, 0);
        EEPROM.put(QUANTITY_3_ADDRESS, DEFAULT_QUANTITTY_3);
        ret = false;
    } else{
        _value.quantity3 = value3;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(QUANTITY_4_ADDRESS, value4);
    /* data validation: */
    if (!(value4 > 0.00) || isnan(value4)){
        _value.quantity4 = DEFAULT_QUANTITTY_4;
        _data_error = VMC_DATA_UNSET;
        DEBUG_INFO_LN("Resetting quantity 4");
        EEPROM.write(QUANTITY_4_ADDRESS, 0);
        EEPROM.put(QUANTITY_4_ADDRESS, DEFAULT_QUANTITTY_4);
        ret = false;
    } else{
        _value.quantity4 = value4;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(QUANTITY_5_ADDRESS, value5);
    /* data validation: */
    if (!(value5 > 0.00) || isnan(value5)){
        _value.quantity5 = DEFAULT_QUANTITTY_5;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(QUANTITY_5_ADDRESS, 0);
        EEPROM.put(QUANTITY_5_ADDRESS, DEFAULT_QUANTITTY_5);
        ret = false;
    } else{
        _value.quantity5 = value5;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(QUANTITY_6_ADDRESS, value6);
    /* data validation: */
    if (!(value6 > 0.00) || isnan(value6)){
        _value.quantity6 = DEFAULT_QUANTITTY_6;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(QUANTITY_6_ADDRESS, 0);
        EEPROM.put(QUANTITY_6_ADDRESS, DEFAULT_QUANTITTY_6);
        ret = false;
    } else{
        _value.quantity6 = value6;
        _data_error = VMC_DATA_OK;
    }

   return ret;
}



