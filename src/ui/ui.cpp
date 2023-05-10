#include "ui.h"
#define TRACE_GROUP "UI"

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
    display1 = new LiquidCrystal_I2C(0x25, 20, 4);
    display2 = new LiquidCrystal_I2C(0x26, 20, 4);
    display1->init();
    display2->init();
    display1->backlight();
    display2->backlight();
    display1->setCursor(0, 1);
    display2->setCursor(0, 1);
    display1->print("Initializing");
    display2->print("Machine...");
    return 0;

}

