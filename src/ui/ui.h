#pragma once
#include "common/common.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "sensors/input/joystick.h"


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
        char* id = nullptr;
        bool loaded = false;

};

LiquidCrystal_I2C * get_display1();
LiquidCrystal_I2C * get_display2();

/* common functions */
void display_net_connected();
void display_net_notConnected();