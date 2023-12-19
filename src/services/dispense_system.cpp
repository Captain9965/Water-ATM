#include "dispense_system.h"
#include "sensors/rfid/rfid.h"
#include"ui/ui.h"

DispenseSystem::DispenseSystem(tap_selection_t tap, uint32_t relay_pin, uint32_t flowmeter_interrupt_pin, float calibration): _tap(tap),\
_flowmeter_interrupt_pin(flowmeter_interrupt_pin), _calibration(calibration){
    _relay = new TStatesActuator(relay_pin, TS_ACTUATOR_OFF);
    // flowmeter interrupt pin setup:
    digitalWrite(_flowmeter_interrupt_pin, INPUT_PULLUP);
    DEBUG_INFO("Calibration set is: "); DEBUG_INFO_LN(_calibration);
    bool result = attachInterruptEx(digitalPinToInterrupt(_flowmeter_interrupt_pin), [this]{this->pulse_count++;}, RISING);
    result ? DEBUG_INFO_LN("Interrupt attached successfully") : DEBUG_INFO_LN("Interrupt failed to attach");
    //load tariff:
    float tariff = 10.0;
    tariff::get_default_instance()->get(&tariff);
    _tariff = tariff;
    DEBUG_INFO("Tariff set is: "); DEBUG_INFO_LN(_tariff);

    float flow_calculation_interval = 0.0;
    FlowCalculationTime::get_default_instance()->get(&flow_calculation_interval);
    // _flow_calculation_interval = flow_calculation_interval;
    DEBUG_INFO("Interval set is: "); DEBUG_INFO_LN(flow_calculation_interval);


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

void DispenseSystem::update_dispensed_quantity(uint32_t time_elapsed){
    _litres_per_min = pulse_count / (2 * _calibration * (time_elapsed / 1000));
    _dispensed_quantity += (_litres_per_min / 60);
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
            get_buzzer()->beep(20);
            _set_state(DISPENSING_STARTING);
           } else{
            set_from_event(DISPENSING_SHOW_DUE_AMOUNT);
           }
           break;
        }
        case DISPENSING_STARTING:
        {
            
            _dispense_timer = millis();
            _quantity_timer = millis();
            _dispensed_quantity = 0.0;
            _set_state(DISPENSING_RUNNING);
            clear_to_event(DISPENSING_STARTING);
            set_from_event(DISPENSING_RUNNING);
            // _relay->on();
            break;
        }
        case DISPENSING_RUNNING:
        {   
            /* is dispensing done? */
            
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
            detachInterruptEx(_flowmeter_interrupt_pin);
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