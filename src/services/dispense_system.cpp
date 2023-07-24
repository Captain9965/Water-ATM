#include "dispense_system.h"

DispenseSystem::DispenseSystem(tap_selection_t tap): _tap(tap){

}

DispenseSystem::~DispenseSystem(){
    stop();
}

// set event from the dispense service: 

void DispenseSystem::set_from_event(dispensing_state_t ev){
    _dispense_system_from_ev_flag |= ev;
}

// set event to the dispense service: 
void DispenseSystem::set_to_event(dispensing_state_t ev){
    _dispense_system_to_ev_flag |= ev;
}

bool DispenseSystem::check_from_event(uint32_t ev){
    return (_dispense_system_from_ev_flag & ev);
}

bool DispenseSystem::check_to_event(uint32_t ev){
    return (_dispense_system_to_ev_flag & ev);
}

void DispenseSystem::clear_from_event(uint32_t ev){
    _dispense_system_from_ev_flag |= ~(ev);
}

void DispenseSystem::clear_to_event(uint32_t ev){
    _dispense_system_to_ev_flag |= ~(ev);
}

dispensing_state_t DispenseSystem::get_dispensing_system_ev(){
    uint32_t temp_state;
    dispensing_state_t s;
    for (int i = 0; i < 18; i++) {
        temp_state = (1 << i);
        if (check_to_event(temp_state)) {
            clear_to_event(temp_state);
            _state = static_cast<dispensing_state_t>(temp_state);
            break;
        }
    }
    s = _state;
    return s;
}

dispensing_state_t DispenseSystem::start(){
    /* clear all dispense system flags and timers:*/
    _select_quantity_timer = millis();
    _payment_timer = millis();
    _dispense_system_from_ev_flag = 0;
    _dispense_system_to_ev_flag = 0;
    _state = DISPENSING_QUANTITY_SELECT;
    return DISPENSING_OK;
}

dispensing_state_t DispenseSystem::run(){
    return DISPENSING_OK;
}
