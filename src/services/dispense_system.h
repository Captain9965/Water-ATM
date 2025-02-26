#pragma once
#include "common/common.h"
#include "actuators/TStatesActuator.h"
#include "attachInterruptEx.h"
#include "vmc/vmc_data.h"
#include "comms/comms_events.h"

typedef enum tap_runnig_flags{
    TAP_1_RUNNING = 1 << 0,
    TAP_2_RUNNING = 1 << 1,
    TAP_3_RUNNING = 1 << 2,
    TAP_4_RUNNING = 1 << 3
}tap_running_flags_t;

typedef enum tap_selection{
    TAP_UNSELECTED,
    DISPENSE_TAP_1,
    DISPENSE_TAP_2,
    DISPENSE_TAP_3,
    DISPENSE_TAP_4,
}tap_selection_t;

typedef enum dispense_status{
    DISPENSE_OK,
    DISPENSE_LOW_FLOW_RATE = -1,
    DISPENSE_HIGH_FLOW_RATE = -2,
    DISPENSE_PAUSED = -3,
    DISPENSE_CANCELLED = -4,
    DISPENSE_CANCELLED_SUCCESS = -5,
    DISPENSE_ERROR = -6
}dispense_status_t;

typedef enum dispensing_states {
    DISPENSING_OK = (1 << 0),
    DISPENSING_QUANTITY_SELECT = (1 << 1),
    DISPENSING_CONFIRMING = (1 << 2),
    DISPENSING_STARTING = (1 << 3),
    DISPENSING_RUNNING = (1 << 4),
    DISPENSING_IDLE = (1 << 5),
    DISPENSING_DONE = (1 << 6),
    DISPENSING_SHOW_DUE_AMOUNT = (1 << 7),
    DISPENSING_PAY_WAIT = (1 << 8),
    DISPENSING_PAY_WAIT_TIMEOUT = (1 << 9),
    DISPENSING_PAY_UPDATE = (1 << 10),
    DISPENSING_EXIT = (1 << 11),
    DISPENSING_ERROR_HIGH_FLOW_RATE = (1 << 12),
    DISPENSING_ERROR_LOW_FLOW_RATE = (1 << 13),
    DISPENSING_INVALID_INPUT = (1 << 14),
    DISPENSING_PAUSED = (1 << 15),
    DISPENSING_CANCELLED_SUCCESS = (1 << 16),
    DISPENSING_CANCELLED = (1 << 17)
} dispensing_state_t;

/* Dispense node: */
class DispenseSystem{
    public:
        DispenseSystem(tap_selection_t tap, uint32_t relay_pin, uint32_t flowmeter_interrupt_pin, float calibration, float tariff);
        ~DispenseSystem();
        dispensing_state_t start();
        dispensing_state_t run();
        dispensing_state_t stop();
        void set_from_event(dispensing_state_t ev);
        void set_to_event(dispensing_state_t ev);
        bool check_from_event(uint32_t ev);
        bool check_to_event(uint32_t ev);
        void clear_to_event(uint32_t ev);
        void clear_from_event(uint32_t ev);
        dispensing_state_t get_dispensing_system_ev();
        void set_dispense_quantity(float quantity);
        float calculate_dispense_quantity(uint32_t cash);
        tap_selection_t get_tap();
        float get_dispensed_quantity();
        dispensing_state_t get_state();
        bool stopped();
        bool dispensing();
        bool paused();
        bool error();
        bool waiting_for_payment();
        DispenseSystem * next = nullptr;
    private:
        void update_dispensed_quantity(uint32_t time_elapsed);
        void _set_state(dispensing_state_t state);
        dispensing_state_t _state = DISPENSING_IDLE;
        tap_selection_t _tap = TAP_UNSELECTED;
        TStatesActuator * _relay = nullptr;
        float _quantity, _dispensed_quantity = 0;
        long long _dispense_time = 0;
        uint32_t _dispense_system_from_ev_flag = 0, _dispense_system_to_ev_flag = 0;
        long long _select_quantity_timer = millis();
        long long _payment_timer = millis();
        long long _dispense_timer = millis(), _quantity_timer = millis(), _flow_calculation_timer = millis(),_dispense_wait_exit_timer = millis(), _state_timer = millis();
        bool is_dispense_tag(String &uid); // to be moved to payment service
        void send_dispense_event();
        volatile byte pulse_count = 0;
        uint32_t _flowmeter_interrupt_pin;
        float _calibration = 10.0; // This represents _pulses per litre
        float _tariff = 10.0; // This is the price per unit
        float _flow_calculation_interval = 1000.0;
        float _litres_per_min = 0.0;
        dispense_status_t _dispense_status = DISPENSE_OK;


};

/* Dispense linked list:
    Used due to 0(n) worst case in traversal, efficient insertion and deletion
  
*/

class dispenseGroup{
    public:
        dispenseGroup();
        ~dispenseGroup();
        DispenseSystem * is_running(tap_selection_t tap);
        bool remove(tap_selection_t tap);
        bool add(tap_selection_t tap);
        DispenseSystem * run();
        bool instances_dispensing();
        static dispenseGroup * get_default_instance();
        bool is_empty();
        uint8_t TAP_RUNNING_FLAG = 0;
        void set_tap_running_flag(tap_running_flags_t flag);
        void clear_tap_running_flag(tap_running_flags_t flag);
        void tap_running_flags_init();
        bool check_tap_running_flag(tap_running_flags_t flag);
    private:
        DispenseSystem * dispenseHead = nullptr;
};
