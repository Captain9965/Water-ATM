#include "dispense_system.h"

DispenseSystem::DispenseSystem(tap_selection_t tap): _tap(tap){

}

DispenseSystem::~DispenseSystem(){
    _dispense_system_from_ev_flag = 0;
    _dispense_system_to_ev_flag = 0;
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
    _dispense_system_from_ev_flag &= ~(ev);
}

void DispenseSystem::clear_to_event(uint32_t ev){
    _dispense_system_to_ev_flag &= ~(ev);
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

void DispenseSystem::_set_state(dispensing_state_t state){
    if (_state != state){
        _state = state;
        /* maybe reset state timers? */
    }
}

dispensing_state_t DispenseSystem::start(){
    /* clear all dispense system flags and timers:*/
    _select_quantity_timer = millis();
    _payment_timer = millis();
    _dispense_system_from_ev_flag = 0;
    _dispense_system_to_ev_flag = 0;
    _set_state(DISPENSING_QUANTITY_SELECT);
    return DISPENSING_OK;
}

void DispenseSystem::set_dispense_quantity(float quantity){
    _quantity = quantity;
}

void DispenseSystem::calculate_dispense_time(){
    /* for test: */
    _dispense_time = 10000;
}

tap_selection_t DispenseSystem::get_tap(){
    return _tap;
}

dispensing_state_t DispenseSystem::run(){

    dispensing_state_t dispense_state = get_dispensing_system_ev();
    switch (dispense_state){
        case DISPENSING_QUANTITY_SELECT:
        {
            set_from_event(DISPENSING_QUANTITY_SELECT);
            _set_state(DISPENSING_IDLE);
            set_from_event(DISPENSING_IDLE);         
            break;
        }
        case DISPENSING_IDLE:
        {
            /* wait for quantity selection: */
            break;
        }
        case DISPENSING_STARTING:
        {
            calculate_dispense_time();
            _dispense_timer = millis();
            _set_state(DISPENSING_RUNNING);
            clear_to_event(DISPENSING_STARTING);
            set_from_event(DISPENSING_RUNNING);
            break;
        }
        case DISPENSING_RUNNING:
        {
            if (millis() - _dispense_timer >= _dispense_time){
                _set_state(DISPENSING_DONE);
            }
            break;
        }
        case DISPENSING_DONE:
        {
            set_from_event(DISPENSING_DONE);
            break;
        }
        case DISPENSING_EXIT:
        {
            /* gracefully exit: */
            clear_from_event(DISPENSING_EXIT);
            break;
        }
        
        default:
            break;
        
    }
    return dispense_state;
}
