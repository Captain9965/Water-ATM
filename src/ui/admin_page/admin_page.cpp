#include "admin_page.h"
#include "../idle_page/idle_page.h"
#include "ui/ui_input.h"
#include "vmc/vmc_data.h"



adminPage::adminPage(){
    id = "ADMIN PAGE";
}

int adminPage::load(){
    uiInput::get_default_instance()->enable_joystick_button();
    uiInput::get_default_instance()->disable_quantity_buttons();
    uiInput::get_default_instance()->disable_tap_buttons();
    clear_displays();
    _state_index = 0;
    admin_page_state = state_array[_state_index];
    tariff::get_default_instance()->get(&_tariff);
    DEBUG_INFO("Current tariff -> "); DEBUG_INFO_LN(_tariff);
    AdminCash::get_default_instance()->get(&_admin_cash);
    DEBUG_INFO("Current AdminCash -> "); DEBUG_INFO_LN(_admin_cash);
    return 0;
}

int adminPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }

    if(move_left()){
        DEBUG_INFO_LN("MOVING LEFT");
        _state_index -= 1;
        if(_state_index < 0){
            _state_index = ADMIN_STATE_ARRAY_SIZE - 1;
        } 
            admin_page_state = state_array[_state_index];
    }

    if(move_right()){
        DEBUG_INFO_LN("MOVING RIGHT");
        _state_index += 1;
        if(_state_index > ADMIN_STATE_ARRAY_SIZE - 1){
            _state_index = 0;
        }
        admin_page_state = state_array[_state_index]; 
    }

    

    switch (admin_page_state)
    {
    case ADMIN_PAGE_LOAD_SET_TARIFF:
    {
        display_info("TARIFF ->");
        DEBUG_INFO_LN("Tariff state");
        admin_page_state = ADMIN_PAGE_SET_TARIFF;
        break;
    }
    case ADMIN_PAGE_SET_TARIFF:
        {
            if(increase()){
                _tariff += 0.1;
            } else if(decrease()){
                _tariff -= 0.1;
            }
            display_value<float>(_tariff);
            break;
        }
    case ADMIN_PAGE_LOAD_SET_ADMINCASH:
    {
        display_info("ADMIN CASH ->");
        DEBUG_INFO_LN("Admin page state");
        admin_page_state = ADMIN_PAGE_SET_ADMINCASH;
        break;
    }
    case ADMIN_PAGE_SET_ADMINCASH:
    {   
        if(increase()){
                _admin_cash += 1;
            } else if(decrease()){
                _admin_cash -= 1;
            }
            display_value<uint32_t>(_admin_cash);
        break;
    }
    case ADMIN_PAGE_FACTORY_RESET:
    {
        break;
    }
    default:
        break;
    }
    
    if(factory_reset()){
        reset_params();
        this->ui->set_page(idlePage::get_default_instance());
        return 0;
    }
    if(exit_admin_state()){
        this->ui->set_page(idlePage::get_default_instance());
        save_params();
        return 0;
    }
    return 0;
}

adminPage* adminPage::get_default_instance(){
    static adminPage page;
    return &page;
}

bool adminPage::exit_admin_state(){
    return uiInput::get_default_instance()->joystick_button_pressed();
}

bool adminPage::factory_reset(){
    return uiInput::get_default_instance()->joystick_button_long_pressed();
}

bool adminPage::increase(){
    return uiInput::get_default_instance()->joystick_up();
}

bool adminPage::decrease(){
    return uiInput::get_default_instance()->joystick_down();
}

bool adminPage::move_right(){
    return uiInput::get_default_instance()->joystick_right();
}

bool adminPage::move_left(){
    return uiInput::get_default_instance()->joystick_left();
}

void adminPage::save_params(){
    clear_displays();
    display_info("Saving...");
    tariff::get_default_instance()->set(_tariff);
    AdminCash::get_default_instance()->set(_admin_cash);
}

void adminPage::reset_params(){
    DEBUG_INFO_LN("Resetting..");
    clear_displays();
    display_info("resetting..");
    /* factory reset*/
    AdminCash::get_default_instance()->set(AdminCash::DEFAULT_ADMIN_CASH);
    tariff::get_default_instance()->set(tariff::DEFAULT_TARIFF);   
}
