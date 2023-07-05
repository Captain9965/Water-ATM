#include "ui.h"

LiquidCrystal_I2C * get_display1(){
    static LiquidCrystal_I2C display1(0x25, 20, 4);
    return &display1;
}

LiquidCrystal_I2C * get_display2(){
    static LiquidCrystal_I2C display2(0x26, 20, 4);
    return &display2;
}

ezBuzzer * get_buzzer(){
    static ezBuzzer buzzer(SYSTEM_BUZZER);
    return &buzzer;
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
    get_buzzer()->loop();
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

/* initialize lcd screen and other driver resources */
int UI::init(){
    DEBUG_INFO_LN("***Initializing Display***");
    get_display1()->init();
    get_display1()->backlight();
    get_display2()->init();
    get_display2()->backlight();
    joystick::get_default_instance()->init();
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
    get_display1()->setCursor(2, 1);
    get_display1()->print("Machine Ready");
    get_display1()->setCursor(2, 2);
    get_display1()->print("Select Tap");
}

void display_info(const char * info){
    get_display1()->clear();
    get_display1()->setCursor(2, 1);
    get_display1()->print("INFO: ");
    get_display1()->setCursor(2, 2);
    get_display1()->print(info);
}

void display_dispenses(float amount1, float amount2, float amount3, float amount4){

    get_display2()->setCursor(1, 0); get_display2()->print("Tap 1: ");
    get_display2()->setCursor(9, 0); get_display2()->print(amount1);
    get_display2()->setCursor(14, 0); get_display2()->print("L");

    get_display2()->setCursor(1, 1); get_display2()->print("Tap 2: ");
    get_display2()->setCursor(9, 1); get_display2()->print(amount2);
    get_display2()->setCursor(14, 1); get_display2()->print("L");

    get_display2()->setCursor(1, 2); get_display2()->print("Tap 3: ");
    get_display2()->setCursor(9, 2); get_display2()->print(amount3);
    get_display2()->setCursor(14, 2); get_display2()->print("L");

    get_display2()->setCursor(1, 3); get_display2()->print("Tap 4: ");
    get_display2()->setCursor(9, 3); get_display2()->print(amount4);
    get_display2()->setCursor(14, 3); get_display2()->print("L");
    
}

void clear_displays(){
    get_display1()->clear();
    get_display2()->clear();
}

void display_time(){
    system_time_t time;
    systemTime::get_default_instance()->getTime(time);
    const char *date_fmt = "%02d/%02d/%02d";
    const char * time_fmt = "%02d:%0d";
    char date_buff[12];
    char time_buff[7];
    sprintf(date_buff, date_fmt,time.day, time.month, time.year);
    sprintf(time_buff, time_fmt, time.hour, time.minutes);
    
    get_display2()->setCursor(2, 1); get_display2()->print(date_buff);
    get_display2()->setCursor(2, 2); get_display2()->print(time_buff);
}

void display_select_quantity(const char * tap){
    get_display1()->setCursor(4, 1);
    get_display1()->print(tap);
    get_display1()->setCursor(1, 2);
    get_display1()->print("Select Quantity");
}

