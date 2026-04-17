#pragma once
#include "ui/ui.h"

class otaPage : public Page {
public:
    otaPage();
    int load()   override;
    int update() override;
    static otaPage* get_default_instance();

private:
    uint8_t _last_pct = 255;   // sentinel — forces first draw
    void draw_progress(uint8_t pct);
};
