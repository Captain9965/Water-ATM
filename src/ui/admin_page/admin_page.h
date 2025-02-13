#pragma once
#include "ui/ui.h"
#include "vmc/vmc_flags.h"
#include "vmc/vmc_data.h"

#define ADMIN_STATE_ARRAY_SIZE 1

typedef enum admin_page_state{
    ADMIN_PAGE_IDLE,
    ADMIN_PAGE_LOAD_SET_TARIFF1,
    ADMIN_PAGE_SET_TARIFF1,
    ADMIN_PAGE_LOAD_SET_TARIFF2,
    ADMIN_PAGE_SET_TARIFF2,
    ADMIN_PAGE_LOAD_SET_TARIFF3,
    ADMIN_PAGE_SET_TARIFF3,
    ADMIN_PAGE_LOAD_SET_TARIFF4,
    ADMIN_PAGE_SET_TARIFF4,
    ADMIN_PAGE_LOAD_SET_ADMINCASH,
    ADMIN_PAGE_SET_ADMINCASH,
    ADMIN_PAGE_FACTORY_RESET
}admin_page_state_t;

class adminPage: public Page{
    public:
        adminPage();
        int load() override;
        int update() override;
        int stop();
        static adminPage* get_default_instance();
    private:
        admin_page_state_t switch_ui_state();
        admin_page_state_t admin_page_state;
        bool exit_admin_state();
        bool factory_reset();
        bool decrease();
        bool increase();
        bool move_left();
        bool move_right();
        void save_params();
        void reset_params();
        template<typename T>
        void adjust_params(T &value, T increment, T max_val,  T min_val);
        tariff_t _tariff = {.tariff1 = 0.0, .tariff2 = 0.0, .tariff3 = 0.0, .tariff4 = 0.0} ;
        uint32_t _admin_cash = 0;
        admin_page_state_t state_array[ADMIN_STATE_ARRAY_SIZE] = {ADMIN_PAGE_LOAD_SET_TARIFF1};
        int8_t _state_index = 0;
};