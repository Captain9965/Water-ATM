#include "ui.h"
#include "comms/comms.h"

byte rssLogo[] = {
  B11111,
  B10101,
  B11111,
  B01110,
  B00100,
  B00100,
  B00100,
  B00100,
};


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
    get_display2()->init();
    get_display2()->backlight();
    get_display1()->backlight();
    joystick::get_default_instance()->init();
    get_display1()->createChar(10,rssLogo);
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

void display_machine_ready(bool clear){
    if(clear)get_display1()->clear();
    get_display1()->setCursor(4, 1);
    get_display1()->print("Select Tap");
}

void display_info(const char * info, bool clear){
    if(clear)get_display1()->clear();
    get_display1()->setCursor(4, 1);
    get_display1()->print(info);
}

void display_secondary_info(const char* info){
    get_display1()->setCursor(4, 2);
    get_display1()->print(info);
}

void display_primary_info(const char* info, bool clear){
    if(clear)get_display1()->clear();
    get_display1()->setCursor(2, 1);
    get_display1()->print(info);
}

void display_primary_info_next_line(const char* info, bool clear){
    if(clear)get_display1()->clear();
    get_display1()->setCursor(2, 2);
    get_display1()->print(info);
}

void display_dispenses(float amount, int tap_index, bool shadow_string){
    switch(tap_index){
        case 1:
            if(shadow_string){
                get_display2()->setCursor(1, 0); get_display2()->print("              ");
            } else {
                get_display2()->setCursor(1, 0); get_display2()->print("Tap 1: ");
                get_display2()->setCursor(9, 0); get_display2()->print(amount);
                get_display2()->setCursor(14, 0); get_display2()->print("L");
            }
            break;
        case 2:
            if(shadow_string){
                get_display2()->setCursor(1, 1); get_display2()->print("              ");
            } else {
                get_display2()->setCursor(1, 1); get_display2()->print("Tap 2: ");
                get_display2()->setCursor(9, 1); get_display2()->print(amount);
                get_display2()->setCursor(14, 1); get_display2()->print("L");
            }
            break;
        case 3:
            if(shadow_string){
                get_display2()->setCursor(1, 2); get_display2()->print("              ");
            } else {
                get_display2()->setCursor(1, 2); get_display2()->print("Tap 3: ");
                get_display2()->setCursor(9, 2); get_display2()->print(amount);
                get_display2()->setCursor(14, 2); get_display2()->print("L");
            }
            break;
        case 4:
            if(shadow_string){
                get_display2()->setCursor(1, 3); get_display2()->print("              ");
            } else {
                get_display2()->setCursor(1, 3); get_display2()->print("Tap 4: ");
                get_display2()->setCursor(9, 3); get_display2()->print(amount);
                get_display2()->setCursor(14, 3); get_display2()->print("L");
            }
            break;
        default:
            break;
    }
}
void update_quantities(float amount, int tap_index){
    switch (tap_index){
        case 1:
            get_display2()->setCursor(9, 0); get_display2()->print(amount);
            break;
        case 2:
            get_display2()->setCursor(9, 1); get_display2()->print(amount);
            break;
        case 3:
            get_display2()->setCursor(9, 2); get_display2()->print(amount);
            break;
        case 4:
            get_display2()->setCursor(9, 3); get_display2()->print(amount);
            break;
        default:
            break;
    }
}

void update_dispense_status(const char * status, int tap_index){
    switch (tap_index){
        case 1:
            get_display2()->setCursor(1, 0); get_display2()->print(status);
            break;
        case 2:
            get_display2()->setCursor(1, 1); get_display2()->print(status);
            break;
        case 3:
            get_display2()->setCursor(1, 2); get_display2()->print(status);
            break;
        case 4:
            get_display2()->setCursor(1, 3); get_display2()->print(status);
            break;
        default:
            break;
    }
}

void clear_displays(){
    get_display1()->clear();
    get_display2()->clear();
}

void display_time(){
    system_time_t time;
    systemTime::get_default_instance()->getTime(time);
    const char *date_fmt = "%02d/%02d/%02d";
    const char * time_fmt = "%02d:%02d";
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

void display_network_strength(){
    
    /* get rss: */
    int16_t rss = Comms::get_instance()->get_rss();
    if (rss < 0){
        rss = 0;
    }

    /* convert rss to percentage given minimum is 0 and maximum is 30*/
    rss = (rss * 100) / 30;
    get_display1()->setCursor(16,0);
    get_display1()->write(byte(10));
    // get_display1()->print("rss:");
    // get_display1()->setCursor(18,0);
    get_display1()->print(rss);
    get_display1()->print("%");
}

