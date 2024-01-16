#include "dispense_system.h"


dispenseGroup::dispenseGroup(){
    if(dispenseHead){
        delete dispenseHead;
        dispenseHead = nullptr;
    }
}

dispenseGroup::~dispenseGroup(){
    if(dispenseHead){
        delete dispenseHead;
        dispenseHead = nullptr;
    }
}

bool dispenseGroup::add(tap_selection_t tap){
    DEBUG_INFO_LN("Creating new node");
    
    uint32_t relay_pin, flow_meter_pin;
    calibration_t calibration_instance;
    tariff_t tariff_instance;
    float tariff = Tariff::DEFAULT_TARIFF_1;
    float calibration = Calibration::DEFAULT_CALIBRATION_1;
    Calibration::get_default_instance()->get(&calibration_instance);
    Tariff::get_default_instance()->get(&tariff_instance);

    switch (tap){
        case DISPENSE_TAP_1:
            relay_pin = SYSTEM_SOLENOID_VALVE_1;
            flow_meter_pin = SYSTEM_SOLENOID_INTERRUPT_1;
            calibration = calibration_instance.calibration1;
            tariff = tariff_instance.tariff1;
            break;
        case DISPENSE_TAP_2:
            relay_pin = SYSTEM_SOLENOID_VALVE_2;        
            flow_meter_pin = SYSTEM_SOLENOID_INTERRUPT_2;
            calibration = calibration_instance.calibration2;
            tariff = tariff_instance.tariff2;
            break;
        case DISPENSE_TAP_3:
            relay_pin = SYSTEM_SOLENOID_VALVE_3;
            flow_meter_pin = SYSTEM_SOLENOID_INTERRUPT_3;
            calibration = calibration_instance.calibration3;
            tariff = tariff_instance.tariff3;
            break;
        case DISPENSE_TAP_4:
            relay_pin = SYSTEM_SOLENOID_VALVE_4;
            flow_meter_pin = SYSTEM_SOLENOID_INTERRUPT_4;
            calibration = calibration_instance.calibration4;
            tariff = tariff_instance.tariff4;
            break;
        default:
            relay_pin = SYSTEM_SOLENOID_VALVE_1;
            flow_meter_pin = SYSTEM_SOLENOID_INTERRUPT_1;
            calibration = calibration_instance.calibration1;
            tariff = tariff_instance.tariff1;
            break;
    }
    /* initialize the new node: */
    DEBUG_INFO("Calibration is : "); DEBUG_INFO_LN(calibration);
    DispenseSystem *  new_dispenseSystem = new DispenseSystem(tap, relay_pin, flow_meter_pin, calibration, tariff);
    if (!new_dispenseSystem){
        return false;
    }

    new_dispenseSystem->start();

    DispenseSystem * last_node = dispenseHead;

    /* next node is still a nullptr: */
    new_dispenseSystem->next = nullptr;

    if (!dispenseHead){
        dispenseHead = new_dispenseSystem;
        return true;
    }

    while(last_node->next)last_node = last_node->next;

    last_node->next = new_dispenseSystem;
    return true;
}

bool dispenseGroup::remove(tap_selection_t tap){
    DispenseSystem * temp = dispenseHead, * prev;

    /* The node is at the beginning: */
    if(temp && temp->get_tap() == tap){
        dispenseHead = temp->next;
        delete temp;
        return true;
    }

    /* find the key to be deleted: */
    while(temp && temp->get_tap() != tap){
        prev = temp;
        temp = temp->next;
    }

    if (!temp){
        return false;
    }

    /* remove the node: */
    prev->next = temp->next;
    delete temp;
    return true;
}

DispenseSystem * dispenseGroup::is_running(tap_selection_t tap){
    DispenseSystem * current = dispenseHead;
    while(current){
        if(current->get_tap() == tap)return current;
        current = current->next;
    }

    return nullptr;
}

bool dispenseGroup::is_empty(){
    if(!dispenseHead) return true;
    return false;
}

DispenseSystem * dispenseGroup::run(){
    static DispenseSystem * current = dispenseHead;
    DispenseSystem *temp = nullptr;
    if(current){
        dispensing_state_t state = current->run();
        temp = current->next;
        if(state == DISPENSING_EXIT){
            DEBUG_INFO_LN("Removing instance");
            remove(current->get_tap());
        }
        if(temp){
            current = temp;
            return current;
        } 
    }
    current = dispenseHead;
    return current;
}

bool dispenseGroup::instances_dispensing(){
    DispenseSystem * current = dispenseHead;
    while (current){
        dispensing_state_t state = current->get_state();
        if (state != DISPENSING_RUNNING){
            return false;
        }
        current = current->next;
    }
    return true;
}

dispenseGroup * dispenseGroup::get_default_instance(){
    static dispenseGroup instance = dispenseGroup();
    return &instance;
}