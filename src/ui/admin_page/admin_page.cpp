#include "admin_page.h"
#include "../idle_page/idle_page.h"
#include "ui/ui_input.h"




adminPage::adminPage(){
    id = "ADMIN PAGE";
}

int adminPage::load(){
    uiInput::get_default_instance()->enable_joystick();
    uiInput::get_default_instance()->disable_quantity_buttons();
    uiInput::get_default_instance()->enable_tap_buttons();
    clear_displays();
    _state_index = 0;
    admin_page_state = state_array[_state_index];
    Tariff::get_default_instance()->get(&_tariff);
    return 0;
}

int adminPage::update(){
    if(!loaded){
        load();
        loaded = true;
    }

    switch_ui_state();
    
    switch (admin_page_state)
    {
    case ADMIN_PAGE_LOAD_SET_TARIFF1:
    {
        display_info("TARIFF 1 ->");
        DEBUG_INFO_LN("Tariff 1 state");
        get_display2()->clear();
        admin_page_state = ADMIN_PAGE_SET_TARIFF1;
        break;
    }
    case ADMIN_PAGE_SET_TARIFF1:
        {
            adjust_params<float>(_tariff.tariff1, 0.1, 100.0, 1.0);
            break;
        }
    case ADMIN_PAGE_LOAD_SET_TARIFF2:
    {
        display_info("TARIFF 2 ->");
        DEBUG_INFO_LN("Tariff 2 state");
        get_display2()->clear();
        admin_page_state = ADMIN_PAGE_SET_TARIFF2;
        break;
    }
    case ADMIN_PAGE_SET_TARIFF2:
        {
            adjust_params<float>(_tariff.tariff2, 0.1, 100.0, 1.0);
            break;
        }
    case ADMIN_PAGE_LOAD_SET_TARIFF3:
    {
        display_info("TARIFF 3 ->");
        DEBUG_INFO_LN("Tariff 3 state");
        get_display2()->clear();
        admin_page_state = ADMIN_PAGE_SET_TARIFF3;
        break;
    }
    case ADMIN_PAGE_SET_TARIFF3:
        {
            adjust_params<float>(_tariff.tariff3, 0.1, 100.0, 1.0);
            break;
        }
    case ADMIN_PAGE_LOAD_SET_TARIFF4:
    {
        display_info("TARIFF 4 ->");
        DEBUG_INFO_LN("Tariff 4 state");
        get_display2()->clear();
        admin_page_state = ADMIN_PAGE_SET_TARIFF4;
        break;
    }
    case ADMIN_PAGE_SET_TARIFF4:
        {
            adjust_params<float>(_tariff.tariff4, 0.1, 100.0, 1.0);
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
    Tariff::get_default_instance()->set(_tariff);
}

void adminPage::reset_params(){
    DEBUG_INFO_LN("Resetting..");
    clear_displays();
    display_info("resetting..");
    /* factory reset all params: */
    Tariff::get_default_instance()->set(_tariff);
    FlowCalculationTime::get_default_instance()->set(FlowCalculationTime::DEFAULT_FLOW_CALCULATION);
    quantities_t quantities= {.quantity1 = Quantities::DEFAULT_QUANTITTY_1,
        .quantity2 = Quantities::DEFAULT_QUANTITTY_2,
        .quantity3 = Quantities::DEFAULT_QUANTITTY_3,
        .quantity4 = Quantities::DEFAULT_QUANTITTY_4,
        .quantity5 = Quantities::DEFAULT_QUANTITTY_5,
        .quantity6 = Quantities::DEFAULT_QUANTITTY_6,  
    };
    Quantities::get_default_instance()->set(quantities);
    calibration_t calibration = {.calibration1 = Calibration::DEFAULT_CALIBRATION_1,
        .calibration2 = Calibration::DEFAULT_CALIBRATION_2,
        .calibration3 = Calibration::DEFAULT_CALIBRATION_3,
        .calibration4 = Calibration::DEFAULT_CALIBRATION_4,
    };
    Calibration::get_default_instance()->set(calibration);

}

template<typename T>
void adminPage::adjust_params(T &value, T increment, T max_val,  T min_val){
    if(increase() && value < max_val - increment){
        get_buzzer()->beep(2);
        value += increment;
        get_display2()->clear();
    } else if(decrease() && value > min_val){
        get_buzzer()->beep(2);
        value -= increment;
        get_display2()->clear();
    }
    display_value<T>(value, false);
}

admin_page_state_t adminPage::switch_ui_state(){
    input_flags_t tap;
    if(uiInput::get_default_instance()->tap_button_pressed(tap)){
        get_buzzer()->beep(20);
        switch (tap){
            case TAP_1_BUTTON:
                admin_page_state = ADMIN_PAGE_LOAD_SET_TARIFF1;
                break;
            case TAP_2_BUTTON:
                admin_page_state = ADMIN_PAGE_LOAD_SET_TARIFF2;
                break;
            case TAP_3_BUTTON:
                admin_page_state = ADMIN_PAGE_LOAD_SET_TARIFF3;
                break;
            case TAP_4_BUTTON:
                admin_page_state = ADMIN_PAGE_LOAD_SET_TARIFF4;
                break;
            default:
                break;
        }
    }

    return admin_page_state;
}
