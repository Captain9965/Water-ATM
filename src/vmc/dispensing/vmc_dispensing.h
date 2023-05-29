#pragma once
#include "vmc/vmc.h"
#include "vmc/vmc_errors.h"

/* state dispensing */
class vmc_dispensing: public State{
    public:
        vmc_dispensing();
        int start() override;
        int stop() override;
        int run() override;
        static vmc_dispensing* get_default_instance();
    private:
        // service_state_t dispensing_state;
        void run_sensors();
        // dehumidifier_state_t run_dehumidifier();
        // rtos::Kernel::Clock::time_point debug_timer = rtos::Kernel::Clock::now();
        
        /* sensors */
        // HeatSinkSensor* _heat_sink_temp_sensor = nullptr;
        // vmc_sensor_state_t _heat_sink_temp_sensor_state = VMC_SENSOR_UNINITIALIZED;

        // AmbientSensor* _ambient_sensor = nullptr;
        // TankDHTSensor* _tank_dht22_sensor = nullptr;
        // vmc_sensor_state_t _ambient_temp_sensor_state = VMC_SENSOR_UNINITIALIZED;

        // /* services*/

        // DispenseSystem* _dispense_service = nullptr;
        // PidDehumidifierSystem* _dehumidifier = nullptr;


};