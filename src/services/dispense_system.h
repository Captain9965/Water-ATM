#pragma once
#include "common/common.h"

typedef enum tap_selection{
    TAP_UNSELECTED,
    DISPENSE_TAP_1,
    DISPENSE_TAP_2,
    DISPENSE_TAP_3,
    DISPENSE_TAP_4,
}tap_selection_t;

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
    DISPENSING_AUTHENTICATING = (1 << 12),
    DISPENSING_ERROR = (1 << 13),
    DISPENSING_INVALID_INPUT = (1 << 14),
    DISPENSING_HALTED = (1 << 15),
    DISPENSING_WAIT_PAY_CANCEL = (1 << 16),
    DISPENSING_CANCELLED = (1 << 17)
} dispensing_state_t;

/* Dispense node: */
class DispenseSystem{
    public:
        DispenseSystem(tap_selection_t tap);
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
        tap_selection_t get_tap();
        float get_dispensed_quantity();
        dispensing_state_t get_state();
        bool stopped();
        DispenseSystem * next = nullptr;
    private:
        void calculate_dispense_time();
        void _set_state(dispensing_state_t state);
        dispensing_state_t _state = DISPENSING_IDLE;
        tap_selection_t _tap = TAP_UNSELECTED;
        float _quantity, _dispensed_quantity = 0;
        long long _dispense_time = 0;
        uint32_t _dispense_system_from_ev_flag = 0, _dispense_system_to_ev_flag = 0;
        long long _select_quantity_timer = millis();
        long long _payment_timer = millis();
        long long _dispense_timer = millis(), _quantity_timer = millis();
        bool is_dispense_tag(String &uid); // to be moved to payment service


};

/* Dispense linked list:
    Used due to 0(n) worst case in traversal 
*/

class dispenseGroup{
    public:
        dispenseGroup();
        ~dispenseGroup();
        bool is_running(tap_selection_t tap);
        bool remove(tap_selection_t tap);
        bool add(tap_selection_t tap);
        DispenseSystem * run();
        bool instances_dispensing();
        static dispenseGroup * get_default_instance();
        bool is_empty();
    private:
        DispenseSystem * dispenseHead = nullptr;
};
