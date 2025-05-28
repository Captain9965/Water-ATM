#include "dispensing_page.h"
#include "vmc/vmc_flags.h"
#include "ui/ui_input.h"
#include "vmc/dispensing/vmc_dispensing.h"
#include "ui/idle_page/idle_page.h"




dispensingPage::dispensingPage(vmc_flags_t initialTap): _initialTap(initialTap){
    id = "IDLE PAGE";
}

int dispensingPage::load(){
   clear_amount();
   clear_displays();
   return 0;
}

int dispensingPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }
   
    _dispense_group = dispenseGroup::get_default_instance();
    _current_dispense_instance = vmc_dispensing::get_default_instance()->get_current_dispense_instance();

     /* which tap button has been pressed to instantiate a dispense activity or*/
    check_for_tap_selection();
    /* instance is running : */
    if (!_dispense_group->is_empty() && _current_dispense_instance){
        switch_ui_state();
        DEBUG_INFO("UI state -> "); DEBUG_INFO_LN(ui_state);
        switch (ui_state){
            case UI_AWAIT_PAYMENT:
                {   
    
                    display_primary_info("Pay via Mpesa", false);
                    break;

                }
            case UI_DISPENSING_STARTING:
                {
                    get_display1()->clear();
                    break;
                }
            case UI_CANCELLED:
                {
                    get_display1()->clear();
                    break;
                }
            case UI_IDLE:
                {
                    update_dispense_quantities();
                    
                    if(_dispense_group->instances_dispensing()){

                        display_machine_ready(false);

                    }
                    break;
                }
            case UI_NOP:
                {
                    break;
                }
            default:
                break;
        }

    } else if (check_vmc_flag(VMC_DISPENSE_DONE)){

        clear_vmc_flag(VMC_DISPENSE_DONE);
        display_machine_ready(true);
        DEBUG_INFO_LN("Dispense group done!");
        ui_state = UI_NOP;
        stop();

    } 

    return 0;
}

int dispensingPage::stop(){
    if (_current_dispense_instance){
        _current_dispense_instance = nullptr;
    }
    if (_dispense_group){
        _dispense_group = nullptr;
    }
    this->ui->set_page(idlePage::get_default_instance());
    return 0;
}

dispensingPage* dispensingPage::get_default_instance(vmc_flags_t initialTap){
    static dispensingPage page(initialTap);
    return &page;
}

void dispensingPage::display_button_pressed_for_dispense(vmc_flags_t & tap){
   switch (tap)
   {
    case VMC_DISPENSE_TAP1:
        display_select_quantity("TAP 1");
        tap = (vmc_flags_t)0;
        break;
    case VMC_DISPENSE_TAP2:
        display_select_quantity("TAP 2");
        tap = (vmc_flags_t)0;
        break;
    case VMC_DISPENSE_TAP3:
        display_select_quantity("TAP 3");
        tap = (vmc_flags_t)0;
        break;
    case VMC_DISPENSE_TAP4:
        display_select_quantity("TAP 4");
        tap = (vmc_flags_t)0;
        break;
   
   default:
    break;
   }
}

// void dispensingPage::check_for_quantity_selection(){
//     /* wait for quantity selection then set dispense system flags and params accordingly: */
//     if (!_current_dispense_instance || !_dispense_group){
//         return;
//     }
//     input_flags_t quantity_tap;
//     if(uiInput::get_default_instance()->quantity_button_pressed(quantity_tap)){
//         get_buzzer()->beep(20);
//         switch (quantity_tap){
//             case QUANTITY_1_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity1);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             case QUANTITY_2_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity2);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             case QUANTITY_3_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity3);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             case QUANTITY_4_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity4);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             case QUANTITY_5_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity5);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             case QUANTITY_6_BUTTON:
//             {
//                 _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
//                 _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
//                 quantities_t quantities;
//                 Quantities::get_default_instance()->get(&quantities);
//                 _current_dispense_instance->set_dispense_quantity(quantities.quantity6);
//                 uiInput::get_default_instance()->disable_quantity_buttons();
//                 get_display1()->clear();
//             }
//                 break;
//             default:
//                 break;
//         }
//     }
// }

void dispensingPage::update_dispense_quantities(){
    if (!_current_dispense_instance || !_dispense_group){
        return;
    }
        
        tap_selection_t tap = _current_dispense_instance->get_tap();
        switch (tap){
            case DISPENSE_TAP_1:
                if (_current_dispense_instance->dispensing()){
                    amount1 = _current_dispense_instance->get_dispensed_quantity();
                    display_dispenses(amount1, 1);
                }
                else if(_current_dispense_instance->paused()){
                    update_dispense_status("Paused", 1);
                }
                else if(_current_dispense_instance->waiting_for_payment()){
                    display_dispenses(amount1, 1, true);
                }
                else if(_current_dispense_instance->error()){
                    update_dispense_status("Low flow error", 1);
                }
                break;
            case DISPENSE_TAP_2:
                if(_current_dispense_instance->dispensing()){
                    amount2 = _current_dispense_instance->get_dispensed_quantity();
                    display_dispenses(amount2, 2);
                }
                else if(_current_dispense_instance->paused()){
                    update_dispense_status("Paused", 2);
                }
                else if(_current_dispense_instance->waiting_for_payment()){
                    display_dispenses(amount2, 2, true);
                }
                else if(_current_dispense_instance->error()){
                    update_dispense_status("Low flow error", 2);
                }
                break;
            case DISPENSE_TAP_3:
                if(_current_dispense_instance->dispensing()){
                    amount3 = _current_dispense_instance->get_dispensed_quantity();
                    display_dispenses(amount3, 3);
                }
                else if(_current_dispense_instance->paused()){
                    update_dispense_status("Paused", 3);
                }
                else if(_current_dispense_instance->waiting_for_payment()){
                    display_dispenses(amount3, 3, true);
                }
                else if(_current_dispense_instance->error()){
                    update_dispense_status("Low flow error", 3);
                }
                break;
            case DISPENSE_TAP_4:
                if(_current_dispense_instance->dispensing()){
                    amount4 = _current_dispense_instance->get_dispensed_quantity();
                    display_dispenses(amount4, 4);
                }
                else if(_current_dispense_instance->paused()){
                    update_dispense_status("Paused", 4);
                }
                else if(_current_dispense_instance->waiting_for_payment()){
                    display_dispenses(amount4, 4, true);
                }
                else if(_current_dispense_instance->error()){
                    update_dispense_status("Low flow error", 4);
                }
                break;
            default:
                break;
        }
}

void dispensingPage::clear_amount(){
    amount1 = amount2 = amount3 = amount4 = 0.0f;
}

/* check for tap selection to determine whether to cancel or launch a new dispense instance*/
void dispensingPage::check_for_tap_selection(){
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        get_buzzer()->beep(20);
        switch (tap){
            case TAP_1_BUTTON:
                {
                    DispenseSystem * running_instance = _dispense_group->is_running(DISPENSE_TAP_1);
                    
                    if(running_instance && running_instance->paused()){
                        running_instance->set_to_event(DISPENSING_RUNNING);
                    }
                    else if(running_instance && running_instance->dispensing()){
                        running_instance->set_to_event(DISPENSING_PAUSED);
                    }
                    // else if (running_instance && running_instance->waiting_for_payment()){
                    //     running_instance->set_to_event(DISPENSING_CANCELLED);
                    //     running_instance->clear_from_event(DISPENSING_IDLE);
                    //     get_display1()->clear();
                    // }
                    else if(_dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_1);
                    }
                } 
                break;
            case TAP_2_BUTTON:
                {
                     DispenseSystem * running_instance = _dispense_group->is_running(DISPENSE_TAP_2);
                    
                     if(running_instance && running_instance->paused()){
                        running_instance->set_to_event(DISPENSING_RUNNING);
                    }
                    else if(running_instance && running_instance->dispensing()){
                        running_instance->set_to_event(DISPENSING_PAUSED);
                    }
                    // else if (running_instance && running_instance->waiting_for_payment()){
                    //     running_instance->set_to_event(DISPENSING_CANCELLED);
                    //     running_instance->clear_from_event(DISPENSING_IDLE);
                    //     get_display1()->clear();
                    // }
                    else if(_dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_2);
                    }
                }
                break;
            case TAP_3_BUTTON:
                {
                     DispenseSystem * running_instance = _dispense_group->is_running(DISPENSE_TAP_3);
                    
                     if(running_instance && running_instance->paused()){
                        running_instance->set_to_event(DISPENSING_RUNNING);
                    }
                    else if(running_instance && running_instance->dispensing()){
                        running_instance->set_to_event(DISPENSING_PAUSED);
                    }
                    // else if (running_instance && running_instance->waiting_for_payment()){
                    //     running_instance->set_to_event(DISPENSING_CANCELLED);
                    //     running_instance->clear_from_event(DISPENSING_IDLE);
                    //     get_display1()->clear();
                    // }
                    else if(_dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_3);
                    }
                } 
                break;
            case TAP_4_BUTTON:
                {
                     DispenseSystem * running_instance = _dispense_group->is_running(DISPENSE_TAP_4);
                    
                     if(running_instance && running_instance->paused()){
                        running_instance->set_to_event(DISPENSING_RUNNING);
                    }
                    else if(running_instance && running_instance->dispensing()){
                        running_instance->set_to_event(DISPENSING_PAUSED);
                    }
                    // else if (running_instance && running_instance->waiting_for_payment()){
                    //     running_instance->set_to_event(DISPENSING_CANCELLED);
                    //     running_instance->clear_from_event(DISPENSING_IDLE);
                    //     get_display1()->clear();
                    // }
                    else if(_dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_4);
                    }
                } 
                break;
            default:
                break;
        }
    }
}

/* switch internal ui state:*/

ui_state_t dispensingPage::switch_ui_state(){
    if(!_current_dispense_instance || !_dispense_group){
        return UI_NOP;
    }

    if(_current_dispense_instance->check_from_event(DISPENSING_PAY_WAIT)){
         _current_dispense_instance->clear_from_event(DISPENSING_PAY_WAIT);

         ui_state = UI_AWAIT_PAYMENT;
    }
    else if(_current_dispense_instance->check_from_event(DISPENSING_STARTING)){
        _current_dispense_instance->clear_from_event(DISPENSING_STARTING);
        ui_state = UI_DISPENSING_STARTING;
    }
    else if (_current_dispense_instance->check_from_event(DISPENSING_CANCELLED_SUCCESS)){
        _current_dispense_instance->clear_from_event(DISPENSING_CANCELLED_SUCCESS);
        ui_state = UI_CANCELLED;
    }
    else if(_current_dispense_instance->check_from_event(DISPENSING_RUNNING)){

        ui_state = UI_IDLE;
    }

    return ui_state;    
}