#include "service_page.h"
#include "../idle_page/idle_page.h"
#include "ui/ui_input.h"



servicePage::servicePage(){
    id = "SERVICE PAGE";
}

int servicePage::load(){
    uiInput::get_default_instance()->enable_joystick();
    uiInput::get_default_instance()->enable_quantity_buttons();
    uiInput::get_default_instance()->disable_tap_buttons();
    _state_index = 0;
    service_page_state = state_array[_state_index];
    Quantities::get_default_instance()->get(&_quantities);
    FlowCalculationTime::get_default_instance()->get(&_flow_calculation_time);
    return 0;
}

int servicePage::update(){
    if(!loaded){
        load();
        loaded = true;
    }

    if(move_left()){
        DEBUG_INFO_LN("MOVING LEFT");
        _state_index -= 1;
        if(_state_index < 0){
            _state_index = SERVICE_STATE_ARRAY_SIZE - 1;
        } 
            service_page_state = state_array[_state_index];
    }

    if(move_right()){
        DEBUG_INFO_LN("MOVING RIGHT");
        _state_index += 1;
        if(_state_index > SERVICE_STATE_ARRAY_SIZE - 1){
            _state_index = 0;
        }
        service_page_state = state_array[_state_index]; 
    }

    switch_ui_state();
    
    switch (service_page_state)
    {
    case SERVICE_PAGE_LOAD_SET_QUANTITY1:
    {
        display_info("QUANTITY 1 ->");
        DEBUG_INFO_LN("Quantity1");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY1;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY1:
        {
            adjust_params<float>(_quantities.quantity1, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_SET_QUANTITY2:
    {
        display_info("QUANTITY 2 ->");
        DEBUG_INFO_LN("Quantity2");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY2;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY2:
        {
            adjust_params<float>(_quantities.quantity2, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_SET_QUANTITY3:
    {
        display_info("QUANTITY 3 ->");
        DEBUG_INFO_LN("Quantity3");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY3;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY3:
        {
            adjust_params<float>(_quantities.quantity3, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_SET_QUANTITY4:
    {
        display_info("QUANTITY 4 ->");
        DEBUG_INFO_LN("Quantity4");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY4;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY4:
        {
            adjust_params<float>(_quantities.quantity4, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_SET_QUANTITY5:
    {
        display_info("QUANTITY 5 ->");
        DEBUG_INFO_LN("Quantity5");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY5;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY5:
        {
            adjust_params<float>(_quantities.quantity5, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_SET_QUANTITY6:
    {
        display_info("QUANTITY 6 ->");
        DEBUG_INFO_LN("Quantity6");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_QUANTITY6;
        break;
    }
    case SERVICE_PAGE_SET_QUANTITY6:
        {
            adjust_params<float>(_quantities.quantity6, 0.1, 100.0, 0.1);
            break;
        }
    case SERVICE_PAGE_LOAD_FLOWCALCULATION:
    {
        display_info("FlowCalculation ->");
        DEBUG_INFO_LN("FlowCalculation");
        get_display2()->clear();
        service_page_state = SERVICE_PAGE_SET_FLOWCALCULATION;
        break;
    }
    case SERVICE_PAGE_SET_FLOWCALCULATION:
        {
            adjust_params<float>(_flow_calculation_time, 0.1, 3000.0, 100.0);
            break;
        }
    default:
        break;
    }
    
    if(exit_admin_state()){
        this->ui->set_page(idlePage::get_default_instance());
        save_params();
        return 0;
    }
    return 0;
}

servicePage* servicePage::get_default_instance(){
    static servicePage page;
    return &page;
}

bool servicePage::exit_admin_state(){
    return uiInput::get_default_instance()->joystick_button_pressed();
}

bool servicePage::factory_reset(){
    return uiInput::get_default_instance()->joystick_button_long_pressed();
}

bool servicePage::increase(){
    return uiInput::get_default_instance()->joystick_up();
}

bool servicePage::decrease(){
    return uiInput::get_default_instance()->joystick_down();
}

bool servicePage::move_right(){
    return uiInput::get_default_instance()->joystick_right();
}

bool servicePage::move_left(){
    return uiInput::get_default_instance()->joystick_left();
}

void servicePage::save_params(){
    clear_displays();
    display_info("Saving...");
    Quantities::get_default_instance()->set(_quantities);
    FlowCalculationTime::get_default_instance()->set(_flow_calculation_time);
}

template<typename T>
void servicePage::adjust_params(T &value, T increment, T max_val,  T min_val){
    if(increase() && value < max_val - increment){
        value += increment;
        get_display2()->clear();
    } else if(decrease() && value > min_val){
        value -= increment;
        get_display2()->clear();
    }
    display_value<T>(value, false);
}

service_page_state_t servicePage::switch_ui_state(){
    input_flags_t quantity;
    if(uiInput::get_default_instance()->quantity_button_pressed(quantity)){
        switch (quantity){
            case QUANTITY_1_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY1;
                break;
            case QUANTITY_2_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY2;
                break;
            case QUANTITY_3_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY3;
                break;
            case QUANTITY_4_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY4;
                break;
            case QUANTITY_5_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY5;
                break;
            case QUANTITY_6_BUTTON:
                service_page_state = SERVICE_PAGE_LOAD_SET_QUANTITY6;
                break;
            default:
                break;
        }
    }

    return service_page_state;
}