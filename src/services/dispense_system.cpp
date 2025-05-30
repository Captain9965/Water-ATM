#include "dispense_system.h"
#include"ui/ui.h"

DispenseSystem::DispenseSystem(tap_selection_t tap, uint32_t relay_pin, uint32_t flowmeter_interrupt_pin, float calibration, float tariff): _tap(tap),\
_flowmeter_interrupt_pin(flowmeter_interrupt_pin), _calibration(calibration), _tariff(tariff){
    /* instantiate relay: */
    _relay = new TStatesActuator(relay_pin, TS_ACTUATOR_OFF);

    // flowmeter interrupt pin setup:
    digitalWrite(_flowmeter_interrupt_pin, INPUT_PULLUP);
    DEBUG_INFO("Calibration set is: "); DEBUG_INFO_LN(_calibration);
    bool result = attachInterruptEx(digitalPinToInterrupt(_flowmeter_interrupt_pin), [this]{this->pulse_count++;}, CHANGE);
    result ? DEBUG_INFO_LN("Interrupt attached successfully") : DEBUG_INFO_LN("Interrupt failed to attach");
    
    float flow_calculation_interval = 0.0;
    FlowCalculationTime::get_default_instance()->get(&flow_calculation_interval);
    _flow_calculation_interval = flow_calculation_interval;
  
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
    _state_timer = millis();
    _dispense_system_from_ev_flag = 0;
    _dispense_system_to_ev_flag = 0;
    _set_state(DISPENSING_PAY_WAIT);
    return DISPENSING_OK;
}

void DispenseSystem::set_dispense_quantity(float quantity){
    _quantity = quantity;
    DEBUG_INFO("Dispensing "); DEBUG_INFO(_quantity); DEBUG_INFO_LN(" L");
    
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

void DispenseSystem::update_dispensed_quantity(float time_elapsed){
    _litres_per_min = (float)pulse_count / (2.0f * _calibration * (time_elapsed / 1000.0f));
    _dispensed_quantity += (_litres_per_min / 60.0f);
}

dispensing_state_t DispenseSystem::run(){

    dispensing_state_t dispense_state = get_dispensing_system_ev();
    switch (dispense_state){

        case DISPENSING_PAY_WAIT:
        {
        //check whether AdmiCash is greater than zero and if so set state to DISPENSE_STARTING
        uint32_t adminCash = 0;
        AdminCash::get_default_instance()->get(&adminCash);
        if(adminCash > 0){
            set_dispense_quantity(calculate_dispense_quantity(adminCash));
            //debug
            DEBUG_INFO("Dispense quantity is: "); DEBUG_INFO_LN(_quantity);
             //reset adminCash
             AdminCash::get_default_instance()->set(0);
            _set_state(DISPENSING_STARTING);
            set_from_event(DISPENSING_STARTING);
        } else {
            set_from_event(DISPENSING_PAY_WAIT);
        }

        /* Exit if the user has not paid in 30 seconds*/
        if (millis() - _state_timer >= 30000){
            _set_state(DISPENSING_EXIT);
        }

        break;
        }
        case DISPENSING_STARTING:
        {
            
            _dispense_timer = millis();
            _quantity_timer = millis();
            _state_timer = millis();
            _dispensed_quantity = 0.0;
            _set_state(DISPENSING_RUNNING);
            clear_to_event(DISPENSING_STARTING);
            set_from_event(DISPENSING_RUNNING);
            break;
        }
        case DISPENSING_RUNNING:
        {   
            _state_timer = millis();
            _relay->on();
            if(millis() - _flow_calculation_timer >= _flow_calculation_interval){
                update_dispensed_quantity(_flow_calculation_interval);
                pulse_count = 0;
                _flow_calculation_timer = millis();
            }

            if(_dispensed_quantity >= _quantity){
                _set_state(DISPENSING_DONE);
            }

            if (millis() - _quantity_timer >= 1000){
                DEBUG_INFO("Dispensed quantity : "); DEBUG_INFO_LN(_dispensed_quantity);
                DEBUG_INFO("L/min : "); DEBUG_INFO_LN(_litres_per_min);
                 _quantity_timer = millis();
            }

            /* if we have dispensed for more than 10 seconds, and the dispense rate in L/min is less than 0.02, go into DISPENSE_ERROR state and exit*/
            if (millis() - _dispense_timer >= 10000 && _litres_per_min < 0.02){
                _set_state(DISPENSING_ERROR_LOW_FLOW_RATE);
                _dispense_wait_exit_timer = millis();
                _relay->off();
            } else if(_litres_per_min > 0.02){
                _dispense_timer = millis();
            }
            break;
        }
        case DISPENSING_CANCELLED:
        {   
            _relay->off();
            DEBUG_INFO_LN("Dispensing cancelled !!");
            clear_to_event(DISPENSING_CANCELLED);
            set_from_event(DISPENSING_CANCELLED_SUCCESS);
            _dispense_status = DISPENSE_CANCELLED;
            _set_state(DISPENSING_EXIT);
            break;
        }
        case DISPENSING_ERROR_LOW_FLOW_RATE:
        {
            
            DEBUG_INFO_LN("Dispensing error: Low flow rate!");
            if(millis() - _dispense_wait_exit_timer >= 5000){
                _dispense_status = DISPENSE_LOW_FLOW_RATE;

                send_dispense_event();
                _set_state(DISPENSING_EXIT);
            }
            break;
        }
        case DISPENSING_PAUSED:
        {
            _relay->off();
            DEBUG_INFO_LN("Dispensing paused!");
            _dispense_timer = millis();
            if(millis() - _state_timer >= 30000){

                _dispense_status = DISPENSE_PAUSED;
                send_dispense_event();
                _set_state(DISPENSING_EXIT);
            }
            break;
        }
        case DISPENSING_DONE:
        {   
            _relay->off();
            DEBUG_INFO("Dispensed quantity is : ");DEBUG_INFO_LN(_dispensed_quantity);
            _dispense_status = DISPENSE_OK;
            send_dispense_event();
            _set_state(DISPENSING_EXIT);
            break;
        }
        case DISPENSING_EXIT:
        {
            /* gracefully exit: */
            detachInterruptEx(_flowmeter_interrupt_pin);
            DEBUG_INFO_LN("Dispensing exiting!");
            break;
        }
        
        default:
            break;
        
    }
    return dispense_state;
}

bool DispenseSystem::stopped(){
    return(_state == DISPENSING_CANCELLED || _state == DISPENSING_EXIT);
 
}

bool DispenseSystem::dispensing(){
    return(_state == DISPENSING_RUNNING);
}

bool DispenseSystem::paused(){
    return(_state == DISPENSING_PAUSED);
}

bool DispenseSystem::error(){
    return(_state == DISPENSING_ERROR_LOW_FLOW_RATE);
}

bool DispenseSystem::waiting_for_payment(){
    return(_state == DISPENSING_PAY_WAIT);
}
float DispenseSystem::calculate_dispense_quantity(uint32_t cash){

    if(_tariff == 0){
        return 0;
    }
    return (float)cash / _tariff;
}

void DispenseSystem::send_dispense_event(){

    dispense_event_t dispense_event;
    dispense_event.tap = _tap;
    dispense_event.amount = _dispensed_quantity;
    dispense_event.status = _dispense_status;
    publish_dispense_event(&dispense_event);
}