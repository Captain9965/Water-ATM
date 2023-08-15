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
   display_dispenses(amount1, amount2, amount3, amount4);
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
        if(_current_dispense_instance->check_from_event(DISPENSING_QUANTITY_SELECT)){

            _current_dispense_instance->clear_from_event(DISPENSING_QUANTITY_SELECT);
            tap_selection_t tap = _current_dispense_instance->get_tap();
            display_info("SELECT QUANTITY");
            char sec_info[15];
            sprintf(sec_info,"FOR TAP %d",tap);
            display_secondary_info(sec_info);
            uiInput::get_default_instance()->enable_quantity_buttons();

        }  else if(_current_dispense_instance->check_from_event(DISPENSING_IDLE)){

            check_for_quantity_selection();

        } else if (_current_dispense_instance->check_from_event(DISPENSING_RUNNING)){

            if(_dispense_group->instances_dispensing()){

                display_net_connected();
            }   

            update_dispense_quantities();

        }   else if (_current_dispense_instance->check_from_event(DISPENSING_SHOW_DUE_AMOUNT)){

            _current_dispense_instance->clear_from_event(DISPENSING_SHOW_DUE_AMOUNT);
            display_info("TAP CARD");

        }
    } else if (check_vmc_flag(VMC_DISPENSE_DONE)){

        clear_vmc_flag(VMC_DISPENSE_DONE);
        DEBUG_INFO_LN("Dispense group done!");
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

void dispensingPage::check_for_quantity_selection(){
    /* wait for quantity selection then set dispense system flags and params accordingly: */
    if (!_current_dispense_instance || !_dispense_group){
        return;
    }
    input_flags_t quantity_tap;
    if(uiInput::get_default_instance()->quantity_button_pressed(quantity_tap)){
        // get_buzzer()->beep(20);
        switch (quantity_tap){
            case QUANTITY_1_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            case QUANTITY_2_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            case QUANTITY_3_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            case QUANTITY_4_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            case QUANTITY_5_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            case QUANTITY_6_BUTTON:
                _current_dispense_instance->set_to_event(DISPENSING_PAY_WAIT);
                _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                uiInput::get_default_instance()->disable_quantity_buttons();
                break;
            default:
                break;
        }
    }
}

void dispensingPage::update_dispense_quantities(){
    if (!_current_dispense_instance || !_dispense_group){
        return;
    }
        tap_selection_t tap = _current_dispense_instance->get_tap();
        switch (tap){
            case DISPENSE_TAP_1:
                amount1 = _current_dispense_instance->get_dispensed_quantity();
                break;
            case DISPENSE_TAP_2:
                amount2 = _current_dispense_instance->get_dispensed_quantity();
                break;
            case DISPENSE_TAP_3:
                amount3 = _current_dispense_instance->get_dispensed_quantity();
                break;
            case DISPENSE_TAP_4:
                amount4 = _current_dispense_instance->get_dispensed_quantity();
                break;
            default:
                break;
        }
        update_quantities(amount1, amount2, amount3, amount4);
   
}

void dispensingPage::clear_amount(){
    amount1 = amount2 = amount3 = amount4 = 0.0f;
}

/* check for tap selection to determine whether to cancel or launch a new dispense instance*/
void dispensingPage::check_for_tap_selection(){
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        // get_buzzer()->beep(20);
        switch (tap){
            case TAP_1_BUTTON:
                {
                    if(_current_dispense_instance && _current_dispense_instance->get_tap() == DISPENSE_TAP_1 && !_current_dispense_instance->stopped()){
                        _current_dispense_instance->set_to_event(DISPENSING_CANCELLED);
                        _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                    }
                     else if(_current_dispense_instance && _dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_1);
                    }
                } 
                break;
            case TAP_2_BUTTON:
                {
                    if(_current_dispense_instance && _current_dispense_instance->get_tap() == DISPENSE_TAP_2 && !_current_dispense_instance->stopped()){
                        _current_dispense_instance->set_to_event(DISPENSING_CANCELLED);
                        _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                    }
                     else if(_current_dispense_instance && _dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_2);
                    }
                } 
                break;
            case TAP_3_BUTTON:
                {
                    if(_current_dispense_instance && _current_dispense_instance->get_tap() == DISPENSE_TAP_3 && !_current_dispense_instance->stopped()){
                        _current_dispense_instance->set_to_event(DISPENSING_CANCELLED);
                        _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                    }
                     else if(_current_dispense_instance && _dispense_group->instances_dispensing()){
                        /* If all instances are running fine, then instantiate another instance*/
                        _dispense_group->add(DISPENSE_TAP_3);
                    }
                } 
                break;
            case TAP_4_BUTTON:
                {
                    if(_current_dispense_instance && _current_dispense_instance->get_tap() == DISPENSE_TAP_4 && !_current_dispense_instance->stopped()){
                        _current_dispense_instance->set_to_event(DISPENSING_CANCELLED);
                        _current_dispense_instance->clear_from_event(DISPENSING_IDLE);
                    }
                     else if(_current_dispense_instance && _dispense_group->instances_dispensing()){
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
