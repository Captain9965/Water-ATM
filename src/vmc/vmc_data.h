#pragma once
#include "common/common.h"
#include "storage/storage.h"

typedef enum vmc_data{
    VMC_DATA_OK = 0, 
    VMC_DATA_OUT_OF_RANGE_LOW = -1,
    VMC_DATA_OUT_OF_RANGE_HIGH = -2, 
    VMC_DATA_UNSET = -3,
    VMC_DATA_SET_ERROR = -4
}vmc_data_error_t;

/* vmc data base class: */
template <typename T>
class vmcData {
    public:
        vmcData(){
        }
        virtual vmc_data_error_t get(T * value){
            assert(value);
            /* maybe lock value??*/
            *value = _value;
            return _data_error;
        }
        virtual vmc_data_error_t set(T value){
            _value = value;
            _data_error = VMC_DATA_OK;
            return _data_error;
        }
        virtual void reset(){
            _value = {};
            _data_error = VMC_DATA_UNSET;
        }
        virtual bool load(){
            return true;
        };
    protected:
        vmc_data_error_t _data_error = VMC_DATA_UNSET;
        T _value = {};
};

/* admin Cash stored in SD card:  */

class AdminCash: public vmcData<uint32_t> {
    public:
        static AdminCash * get_default_instance();
        virtual vmc_data_error_t set(uint32_t value);
        virtual bool load();
        static uint32_t DEFAULT_ADMIN_CASH;
};

/* Tariff stored in EEPROM: */

class tariff: public vmcData<float> {
    public:
        static tariff * get_default_instance();
        virtual vmc_data_error_t set(float value);
        virtual bool load();
        static float DEFAULT_TARIFF;
};


/* Calibration stored in EEPROM: */

class Calibration: public vmcData<float> {
    public:
        static Calibration * get_default_instance();
        virtual vmc_data_error_t set(float value);
        virtual bool load();
        static float DEFAULT_CALIBRATION;
};

/* Flow calculation stored in EEPROM: */

class FlowCalculationTime: public vmcData<float> {
    public:
        static FlowCalculationTime * get_default_instance();
        virtual vmc_data_error_t set(float value);
        virtual bool load();
        static float DEFAULT_FLOW_CALCULATION;
};

/* Quantities struct: */
typedef struct quantities
{
    float quantity1;
    float quantity2;
    float quantity3;
    float quantity4;
    float quantity5;
    float quantity6;

}quantities_t;


class Quantities: public vmcData<quantities_t> {
    public:
        static Quantities * get_default_instance();
        virtual vmc_data_error_t set(quantities_t value);
        virtual bool load();
        static float DEFAULT_QUANTITTY_1;
        static float DEFAULT_QUANTITTY_2;
        static float DEFAULT_QUANTITTY_3;
        static float DEFAULT_QUANTITTY_4;
        static float DEFAULT_QUANTITTY_5;
        static float DEFAULT_QUANTITTY_6;
};
