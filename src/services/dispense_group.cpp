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
    /* initialize the new node: */
    uint32_t relay_pin;
    switch (tap){
        case DISPENSE_TAP_1:
            relay_pin = SYSTEM_SOLENOID_VALVE_1;
            break;
        case DISPENSE_TAP_2:
            relay_pin = SYSTEM_SOLENOID_VALVE_2;
            break;
        case DISPENSE_TAP_3:
            relay_pin = SYSTEM_SOLENOID_VALVE_3;
            break;
        case DISPENSE_TAP_4:
            relay_pin = SYSTEM_SOLENOID_VALVE_4;
            break;
        default:
            relay_pin = SYSTEM_SOLENOID_VALVE_1;
            break;
    }
    DispenseSystem *  new_dispenseSystem = new DispenseSystem(tap, relay_pin);
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

bool dispenseGroup::is_running(tap_selection_t tap){
    DispenseSystem * current = dispenseHead;
    while(current){
        if(current->get_tap() == tap)return true;
        current = current->next;
    }

    return false;
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