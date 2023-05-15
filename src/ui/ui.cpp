#include "ui.h"

LiquidCrystal_I2C * get_display1(){
    static LiquidCrystal_I2C display1(0x25, 20, 4);
    return &display1;
}

LiquidCrystal_I2C * get_display2(){
    static LiquidCrystal_I2C display2(0x26, 20, 4);
    return &display2;
}

int Page::set_ui_context(UI* ui){
    assert(ui != nullptr);
    this->ui = ui;
    return 0;
}

int Page::load(){
    loaded = true;
    DEBUG_INFO_LN("Loading default page..");
    return 0;
}

int Page::update(){
    if(!loaded){
        load();
    }
    DEBUG_INFO_LN("Updating default page..");
    return 0;
}

UI::UI(){
    
}

UI::~UI(){

}

int UI::load(){
    this->ui_page->load();
    return 0;
}

int UI::update(){
    this->ui_page->update();
    return 0;
}

int UI::set_page(Page* page){
    assert(page != nullptr);
    DEBUG_INFO("Transitioning page to->  ");
    DEBUG_INFO_LN(page->id);
    page->loaded = false;
    this->ui_page = page;
    this->ui_page->set_ui_context(this);
    return 0;
}

UI* UI::get_default_instance(){
   static UI ui;
   return &ui;

}

Page* UI::get_page(){
    return ui_page;
}

void UI::tick(){
    while(1){
        update();
        /* delay thread here*/
    }
}

/* initialize lvgl and driver resources */
int UI::init(){
    DEBUG_INFO_LN("***Initializing Display***");
    get_display1()->init();
    get_display1()->backlight();
    return 0;

}


/* common functions: */

void display_net_notConnected(){
    get_display1()->clear();
    get_display1()->setCursor(2, 1);
    get_display1()->print("Connecting");
    get_display1()->setCursor(2, 2);
    get_display1()->print("Please wait....");

};

void display_net_connected(){
    get_display1()->clear();
    get_display1()->setCursor(2, 2);
    get_display1()->print("Machine ready!!");
}

