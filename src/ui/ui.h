#pragma once
#include "common/common.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "sensors/input/joystick.h"
#include "ezBuzzer.h"
#include "sensors/rtc/rtc.h"


/* This is the UI state Machine (WIP) */
class Page;
class UI{
    protected:
        Page* ui_page = nullptr;
        /* lcd instances: */
        

    public:
        UI();
        ~UI();
        virtual int load();
        virtual int update();
        virtual int set_page(Page* page);
        virtual Page* get_page();
        static UI* get_default_instance();
        virtual int init();
        virtual void tick(); 
    private:
        /* ui thread here...if we need one.*/
};

class Page{
    protected:
        UI* ui = nullptr;
    public:
        virtual int load();
        virtual int update();
        virtual int set_ui_context(UI* ui);
        const char* id = nullptr;
        bool loaded = false;

};

LiquidCrystal_I2C * get_display1();
LiquidCrystal_I2C * get_display2();

ezBuzzer * get_buzzer();

/* common functions */
void display_machine_ready(bool clear = true);
void display_net_notConnected();
void display_info(const char * info, bool clear = true);
void display_primary_info(const char* info, bool clear = true);
void display_primary_info_next_line(const char* info, bool clear = true);
void display_secondary_info(const char* info);
void display_dispenses(float amount, int tap_index, bool shadow_string = false);
void update_quantities(float amount, int tap_index);
void update_dispense_status(const char * status, int tap_index);
void clear_displays();
void display_time();
void display_select_quantity(const char * tap);
template <typename T>
void display_value(T val, bool clear = true){
    if(clear)get_display2()->clear();
    get_display2()->setCursor(4, 1);
    get_display2()->print(val);
}
void display_network_strength();