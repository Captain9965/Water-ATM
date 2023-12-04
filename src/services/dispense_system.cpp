#include "dispense_system.h"
#include "sensors/rfid/rfid.h"

DispenseSystem::DispenseSystem(tap_selection_t tap, uint32_t relay_pin): _tap(tap){
    _relay = new TStatesActuator(relay_pin, TS_ACTUATOR_OFF);
}

DispenseSystem::~DispenseSystem(){
    _dispense_system_from_ev_flag = 0;
    _dispense_system_to_ev_flag = 0;

    if(_relay){
        delete _relay;
        DEBUG_INFO_LN("Relay instance deallocated");
    }
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
    _dispense_time = 20000;
}

tap_selection_t DispenseSystem::get_tap(){
    return _tap;
}

float DispenseSystem::get_dispensed_quantity(){
    return _dispensed_quantity;
}
dispensing_state_t DispenseSystem::get_state(){
    return _state;
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
        case DISPENSING_PAY_WAIT:
        {
            /** rfid payment
            *@todo replace this with payment instance for MPESA....
            */
           String uid = RFID::get_default_instance()->read_uid();
           if (is_dispense_tag(uid)){
            _set_state(DISPENSING_STARTING);
           } else{
            set_from_event(DISPENSING_SHOW_DUE_AMOUNT);
           }
           break;
        }
        case DISPENSING_STARTING:
        {
            calculate_dispense_time();
            _dispense_timer = millis();
            _quantity_timer = millis();
            _dispensed_quantity = 0.0;
            _set_state(DISPENSING_RUNNING);
            clear_to_event(DISPENSING_STARTING);
            set_from_event(DISPENSING_RUNNING);
            _relay->on();
            break;
        }
        case DISPENSING_RUNNING:
        {
            if (millis() - _dispense_timer >= _dispense_time){
                _set_state(DISPENSING_DONE);
            }

            if (millis() - _quantity_timer >= 500){
                 _dispensed_quantity += 0.1;
                 _quantity_timer = millis();
            }
           
            break;
        }
        case DISPENSING_CANCELLED:
        {   
            _relay->off();
            DEBUG_INFO_LN("Dispensing cancelled !!");
            clear_to_event(DISPENSING_CANCELLED);
            _set_state(DISPENSING_EXIT);
            break;
        }
        case DISPENSING_DONE:
        {   
            _relay->off();
            DEBUG_INFO("Dispensed quantity is : ");DEBUG_INFO_LN(_dispensed_quantity);
            _set_state(DISPENSING_EXIT);
            break;
        }
        case DISPENSING_EXIT:
        {
            /* gracefully exit: */
            DEBUG_INFO_LN("Dispensing exiting!");
            break;
        }
        
        default:
            break;
        
    }
    return dispense_state;
}

bool DispenseSystem::is_dispense_tag(String &uid){
    if (uid.substring(1) == VENDING_CARD)
    {
        return true;
    }
    return false;
}

bool DispenseSystem::stopped(){
    return(_state == DISPENSING_CANCELLED || _state == DISPENSING_EXIT);
 
}