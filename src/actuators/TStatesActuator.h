#pragma once
#include "common/common.h"


typedef enum {
    TS_ACTUATOR_OFF = 0, /*!< state 0 */
    TS_ACTUATOR_ON = 1 /*!< state 1 */
} ts_actuator_state_t;

/**
 * @brief Generic two states (on/off) actuator using DigitalOut as control system.
 *
 */
class TStatesActuator {
public:
    /**
     * @brief Construct a new TStatesActuator object
     *
     * @param pinOut PinName to initialise the actuator
     * @param default_state during initialisation the state immediately
     */
    TStatesActuator(uint32_t pinOut, ts_actuator_state_t default_state, bool inverted = false): actuatorPin(pinOut), inverted(inverted)
    {   
        pinMode(actuatorPin, OUTPUT);
        digitalWrite(actuatorPin, (uint32_t)default_state);
    }

  
    /**
     * @brief
     *
     * @return ts_actuator_state_t
     */
    ts_actuator_state_t on()
    {
        digitalWrite(actuatorPin,!inverted ? (uint32_t)TS_ACTUATOR_ON : (uint32_t)TS_ACTUATOR_OFF);
        return !inverted ? TS_ACTUATOR_ON : TS_ACTUATOR_OFF;
    }

    /**
     * @brief
     *
     * @return ts_actuator_state_t
     */
    ts_actuator_state_t off()
    {
        digitalWrite(actuatorPin,!inverted ? (uint32_t)TS_ACTUATOR_OFF : (uint32_t)TS_ACTUATOR_ON);
        return !inverted ? TS_ACTUATOR_OFF : TS_ACTUATOR_ON;
    }

    /**
     * @brief
     *
     * @param state
     * @return ts_actuator_state_t
     */
    ts_actuator_state_t write(ts_actuator_state_t state)
    {
        digitalWrite(actuatorPin,(uint32_t)state);
        return state;
    }
    /**
     * @brief
     *
     * @return ts_actuator_state_t
     */

private:
    uint32_t actuatorPin;
    bool inverted = false;
};
