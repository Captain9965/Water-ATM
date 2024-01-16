#pragma once
#include "ui/ui.h"
#include "vmc/vmc_flags.h"
#include "vmc/vmc_data.h"

#define SERVICE_STATE_ARRAY_SIZE 2

typedef enum service_page_state{
    SERVICE_PAGE_IDLE,
    SERVICE_PAGE_LOAD_SET_QUANTITY1,
    SERVICE_PAGE_SET_QUANTITY1,
    SERVICE_PAGE_LOAD_SET_QUANTITY2,
    SERVICE_PAGE_SET_QUANTITY2,
    SERVICE_PAGE_LOAD_SET_QUANTITY3,
    SERVICE_PAGE_SET_QUANTITY3,
    SERVICE_PAGE_LOAD_SET_QUANTITY4,
    SERVICE_PAGE_SET_QUANTITY4,
    SERVICE_PAGE_LOAD_SET_QUANTITY5,
    SERVICE_PAGE_SET_QUANTITY5,
    SERVICE_PAGE_LOAD_SET_QUANTITY6,
    SERVICE_PAGE_SET_QUANTITY6,
    SERVICE_PAGE_LOAD_FLOWCALCULATION,
    SERVICE_PAGE_SET_FLOWCALCULATION
}service_page_state_t;

class servicePage: public Page{
    public:
        servicePage();
        int load() override;
        int update() override;
        int stop();
        static servicePage* get_default_instance();
    private:
        service_page_state_t switch_ui_state();
        service_page_state_t service_page_state;
        bool exit_admin_state();
        bool factory_reset();
        bool decrease();
        bool increase();
        bool move_left();
        bool move_right();
        void save_params();
        template<typename T>
        void adjust_params(T &value, T increment, T max_val,  T min_val);
        quantities_t _quantities = {.quantity1 = 0.0, .quantity2 = 0.0, .quantity3 = 0.0, .quantity4 = 0.0, .quantity5 = 0.0, .quantity6 = 0.0};
        float _flow_calculation_time = 0.0;
        service_page_state_t state_array[SERVICE_STATE_ARRAY_SIZE] = {SERVICE_PAGE_LOAD_SET_QUANTITY1, SERVICE_PAGE_LOAD_FLOWCALCULATION};
        int8_t _state_index = 0;
};