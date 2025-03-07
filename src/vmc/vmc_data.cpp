#include "vmc_data.h"


AdminCash * AdminCash::get_default_instance(){
    static AdminCash _instance = AdminCash();
    return &_instance;
}

vmc_data_error_t AdminCash::set(uint32_t value){
    /* data validation: */
    _value = value;
    if (!storage::get_default_instance()->writeValue(1, 1, String(value))){
        _value = value;
        _data_error = VMC_DATA_SET_ERROR;
        return _data_error;
    }

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

float FlowCalculationTime::DEFAULT_FLOW_CALCULATION = 1000.0;

FlowCalculationTime * FlowCalculationTime::get_default_instance(){
    static FlowCalculationTime _instance = FlowCalculationTime();
    return &_instance;
}

vmc_data_error_t FlowCalculationTime::set(float value){
 /* data validation: */
    EEPROM.put(FLOW_CALCULATION_TIME_ADDRESS, value);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

bool FlowCalculationTime::load(){
    float value = 0.00;
    EEPROM.get(FLOW_CALCULATION_TIME_ADDRESS, value);
    /* data validation: */
    if (!(value > 0.00) || isnan(value)){
        _value = DEFAULT_FLOW_CALCULATION;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(FLOW_CALCULATION_TIME_ADDRESS, DEFAULT_FLOW_CALCULATION);
        return false;
    }

    _value = value;
    _data_error = VMC_DATA_OK;
    return true;
}

float Quantities::DEFAULT_QUANTITTY_1 = 0.1;
float Quantities::DEFAULT_QUANTITTY_2 = 0.2;
float Quantities::DEFAULT_QUANTITTY_3 = 0.3;
float Quantities::DEFAULT_QUANTITTY_4 = 0.4;
float Quantities::DEFAULT_QUANTITTY_5 = 0.5;
float Quantities::DEFAULT_QUANTITTY_6 = 0.6;

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


float Calibration::DEFAULT_CALIBRATION_1 = 45.0;
float Calibration::DEFAULT_CALIBRATION_2 = 45.0;
float Calibration::DEFAULT_CALIBRATION_3 = 45.0;
float Calibration::DEFAULT_CALIBRATION_4 = 45.0;

Calibration * Calibration::get_default_instance(){
    static Calibration _instance = Calibration();
    return &_instance;
}

vmc_data_error_t Calibration::set(calibration_t value){
 /* see whether value has changed and if so, change setting: */
 if(value.calibration1 != _value.calibration1){
    /* data validation: */
    DEBUG_INFO_LN("Calibration1 has changed");
    EEPROM.put(CALIBRATION_1_ADDRESS, value.calibration1);
    _value.calibration1 = value.calibration1;
    _data_error = VMC_DATA_OK;
 }

 if(value.calibration2 != _value.calibration2){
    /* data validation: */
    DEBUG_INFO_LN("Calibration2 has changed");
    EEPROM.put(CALIBRATION_2_ADDRESS, value.calibration2);
    _value.calibration2 = value.calibration2;
    _data_error = VMC_DATA_OK;
 }

 if(value.calibration3 != _value.calibration3){
    /* data validation: */
    DEBUG_INFO_LN("Calibration3 has changed");
    EEPROM.put(CALIBRATION_3_ADDRESS, value.calibration3);
    _value.calibration3 = value.calibration3;
    _data_error = VMC_DATA_OK;
 }
 if(value.calibration4 != _value.calibration4){
    /* data validation: */
    DEBUG_INFO_LN("Calibration4 has changed");
    EEPROM.put(CALIBRATION_4_ADDRESS, value.calibration4);
    _value.calibration4 = value.calibration4;
    _data_error = VMC_DATA_OK;
 }
 return _data_error;
    
}

bool Calibration::load(){
    bool ret = true;
    float value1 = 0.00;
    float value2 = 0.00;
    float value3 = 0.00;
    float value4 = 0.00;
 

    EEPROM.get(CALIBRATION_1_ADDRESS, value1);
    /* data validation: */
    if (!(value1 > 0.00) || isnan(value1)){
        _value.calibration1 = DEFAULT_CALIBRATION_1;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(CALIBRATION_1_ADDRESS, 0);
        EEPROM.put(CALIBRATION_1_ADDRESS, DEFAULT_CALIBRATION_1);
        ret = false;
    } else{
        _value.calibration1 = value1;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(CALIBRATION_2_ADDRESS, value2);
    /* data validation: */
    if (!(value2 > 0.00) || isnan(value2)){
        _value.calibration2 = DEFAULT_CALIBRATION_2;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(CALIBRATION_2_ADDRESS, 0);
        EEPROM.put(CALIBRATION_2_ADDRESS, DEFAULT_CALIBRATION_2);
        ret = false;
    } else{
        _value.calibration2 = value2;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(CALIBRATION_3_ADDRESS, value3);
    /* data validation: */
    if (!(value3 > 0.00) || isnan(value3)){
        _value.calibration3 = DEFAULT_CALIBRATION_3;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(CALIBRATION_3_ADDRESS, 0);
        EEPROM.put(CALIBRATION_3_ADDRESS, DEFAULT_CALIBRATION_3);
        ret = false;
    } else{
        _value.calibration3 = value3;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(CALIBRATION_4_ADDRESS, value4);
    /* data validation: */
    if (!(value4 > 0.00) || isnan(value4)){
        _value.calibration4 = DEFAULT_CALIBRATION_4;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(CALIBRATION_4_ADDRESS, 0);
        EEPROM.put(CALIBRATION_4_ADDRESS, DEFAULT_CALIBRATION_4);
        ret = false;
    } else{
        _value.calibration4 = value4;
        _data_error = VMC_DATA_OK;
    }
   return ret;
}

/* Tariffs:*/
float Tariff::DEFAULT_TARIFF_1 = 10.0;
float Tariff::DEFAULT_TARIFF_2 = 10.0;
float Tariff::DEFAULT_TARIFF_3 = 10.0;
float Tariff::DEFAULT_TARIFF_4 = 10.0;

Tariff * Tariff::get_default_instance(){
    static Tariff _instance = Tariff();
    return &_instance;
}

vmc_data_error_t Tariff::set(tariff_t value){
 /* see whether value has changed and if so, change setting: */
 if(value.tariff1 != _value.tariff1){
    /* data validation: */
    DEBUG_INFO_LN("Tariff1 has changed");
    EEPROM.put(TARIFF_1_ADDRESS, value.tariff1);
    _value.tariff1 = value.tariff1;
    _data_error = VMC_DATA_OK;
 }

 if(value.tariff2 != _value.tariff2){
    /* data validation: */
    DEBUG_INFO_LN("Tariff2 has changed");
    EEPROM.put(TARIFF_2_ADDRESS, value.tariff2);
    _value.tariff2 = value.tariff2;
    _data_error = VMC_DATA_OK;
 }

 if(value.tariff3 != _value.tariff3){
    /* data validation: */
    DEBUG_INFO_LN("Tariff3 has changed");
    EEPROM.put(TARIFF_3_ADDRESS, value.tariff3);
    _value.tariff3 = value.tariff3;
    _data_error = VMC_DATA_OK;
 }

 if(value.tariff4 != _value.tariff4){
    /* data validation: */
    DEBUG_INFO_LN("Tariff4 has changed");
    EEPROM.put(TARIFF_4_ADDRESS, value.tariff4);
    _value.tariff4 = value.tariff4;
    _data_error = VMC_DATA_OK;
 }

 
 return _data_error;
    
}

bool Tariff::load(){
    bool ret = true;
    float value1 = 0.00;
    float value2 = 0.00;
    float value3 = 0.00;
    float value4 = 0.00;
 

    EEPROM.get(TARIFF_1_ADDRESS, value1);
    /* data validation: */
    if (!(value1 > 0.00) || isnan(value1)){
        _value.tariff1 = DEFAULT_TARIFF_1;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(TARIFF_1_ADDRESS, 0);
        EEPROM.put(TARIFF_1_ADDRESS, DEFAULT_TARIFF_1);
        ret = false;
    } else{
        _value.tariff1 = value1;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(TARIFF_2_ADDRESS, value2);
    /* data validation: */
    if (!(value2 > 0.00) || isnan(value2)){
        _value.tariff2 = DEFAULT_TARIFF_2;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(TARIFF_2_ADDRESS, 0);
        EEPROM.put(TARIFF_2_ADDRESS, DEFAULT_TARIFF_2);
        ret = false;
    } else{
        _value.tariff2 = value2;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(TARIFF_3_ADDRESS, value3);
    /* data validation: */
    if (!(value3 > 0.00) || isnan(value3)){
        _value.tariff3 = DEFAULT_TARIFF_3;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(TARIFF_3_ADDRESS, 0);
        EEPROM.put(TARIFF_3_ADDRESS, DEFAULT_TARIFF_3);
        ret = false;
    } else{
        _value.tariff3 = value3;
        _data_error = VMC_DATA_OK;
    }

    EEPROM.get(TARIFF_4_ADDRESS, value4);
    /* data validation: */
    if (!(value4 > 0.00) || isnan(value4)){
        _value.tariff4 = DEFAULT_TARIFF_4;
        _data_error = VMC_DATA_UNSET;
        EEPROM.write(TARIFF_4_ADDRESS, 0);
        EEPROM.put(TARIFF_4_ADDRESS, DEFAULT_TARIFF_4);
        ret = false;
    } else{
        _value.tariff4 = value4;
        _data_error = VMC_DATA_OK;
    }

    
   return ret;
}
/*define AdminCard class methods*/
AdminCard * AdminCard::get_default_instance(){
    static AdminCard _instance = AdminCard();
    return &_instance;
}

vmc_data_error_t AdminCard::set(String value){
    /* set value in EEPROM*/
    char valueArray[12];
    value.toCharArray(valueArray, 12);
    EEPROM.put(ADMIN_CARD_ADDRESS, valueArray);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

String AdminCard::DEFAULT_ADMIN_CARD = "80 D1 BD 2B";

bool AdminCard::load(){
    /*load value from EEPROM*/
    String value;
    char valueArray[12];
    EEPROM.get(ADMIN_CARD_ADDRESS, valueArray);
    value = String(valueArray);
    /* data validation,consider if a string is empty */ 
    if (value.length() == 0 || value == " "){
        _value = DEFAULT_ADMIN_CARD;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(ADMIN_CARD_ADDRESS, DEFAULT_ADMIN_CARD);
        return false;
    }
    _value = value;
    return true;
}

/*define ServiceCard class methods*/

ServiceCard * ServiceCard::get_default_instance(){
    static ServiceCard _instance = ServiceCard();
    return &_instance;
}

vmc_data_error_t ServiceCard::set(String value){
    /* set value in EEPROM*/
    char valueArray[12];
    value.toCharArray(valueArray, 12);
    EEPROM.put(SERVICE_CARD_ADDRESS, valueArray);
    _value = value;
    _data_error = VMC_DATA_OK;
    return _data_error;
}

String ServiceCard::DEFAULT_SERVICE_CARD = "F3 79 B3 18";

bool ServiceCard::load(){
    /*load value from EEPROM*/
    String value;
    char valueArray[12];
    EEPROM.get(SERVICE_CARD_ADDRESS, valueArray);
    value = String(valueArray);
    /* data validation: */ 
    if (value.length() == 0 || value == " "){
        _value = DEFAULT_SERVICE_CARD;
        _data_error = VMC_DATA_UNSET;
        EEPROM.put(SERVICE_CARD_ADDRESS, DEFAULT_SERVICE_CARD);
        return false;
    }
    _value = value;
    return true;
}

/* reset method for Service card*/
void ServiceCard::reset(){
    _value = DEFAULT_SERVICE_CARD;
    _data_error = set(DEFAULT_SERVICE_CARD);
}

/*rest method for Admin Card*/
void AdminCard::reset(){
    _value = DEFAULT_ADMIN_CARD;
    _data_error = set(DEFAULT_ADMIN_CARD);
}

