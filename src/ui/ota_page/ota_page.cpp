#include "ota_page.h"
#include "ota/mqtt_ota.h"
#include "vmc/vmc_flags.h"
#include "ui/idle_page/idle_page.h"
#include "ui/ui_input.h"

otaPage::otaPage() {
    id = "OTA PAGE";
}

// Called once on entry.  Sets up the static text on both displays.
//
// Display 1  (20 x 4)
//   row 0: "                    "
//   row 1: "  Firmware Update   "
//   row 2: "   Please wait...   "
//   row 3: "                    "
//
// Display 2 — cleared here; progress is drawn by draw_progress().

int otaPage::load() {
    _last_pct = 255;   // force a full redraw on the first update()

    get_display1()->clear();
    get_display1()->setCursor(2, 1);
    get_display1()->print("Firmware Update");
    get_display1()->setCursor(3, 2);
    get_display1()->print("Please wait...");

    get_display2()->clear();
    uiInput::get_default_instance()->disable_tap_buttons();
    return 0;
}

// Called every loop cycle.  Redraws display 2 whenever the percentage changes.
int otaPage::update() {
    if (!loaded) {
        load();
        loaded = true;
    }

    MqttOtaHandler* ota = MqttOtaHandler::get_instance();
    ota->tick();

    // Transfer failed, timed out, or was aborted — re-enable buttons and return to idle.
    if (!ota->is_active()) {
        uiInput::get_default_instance()->enable_tap_buttons();
        clear_displays();
        this->ui->set_page(idlePage::get_default_instance());
        return 0;
    }

    uint8_t pct = ota->get_progress_pct();

    if (pct != _last_pct) {
        _last_pct = pct;
        draw_progress(pct);
    }

    return 0;
}

// Draws the progress bar and percentage on display 2.
//
// Display 2  (20 x 4)
//   row 0: "[================  ]"   (18-char bar between [ and ])
//   row 1: "        50%         "
void otaPage::draw_progress(uint8_t pct) {
    char buf[21];

    // ── row 0: progress bar ────────────────────────────────────────────────
    // "[" + 18 fill chars + "]" = 20 chars
    buf[0] = '[';
    uint8_t filled = (uint8_t)((pct * 18UL) / 100);
    for (uint8_t i = 0; i < 18; i++) {
        buf[1 + i] = (i < filled) ? '=' : ' ';
    }
    buf[19] = ']';
    buf[20] = '\0';
    get_display2()->setCursor(0, 0);
    get_display2()->print(buf);

    // ── row 1: percentage (centred) ───────────────────────────────────────
    // "        XX%         " — 8 leading spaces keep it centred for 1–3 digit pct
    int n = snprintf(buf, sizeof(buf), "        %3u%%", pct);
    while (n < 20) buf[n++] = ' ';
    buf[20] = '\0';
    get_display2()->setCursor(0, 1);
    get_display2()->print(buf);

}

otaPage* otaPage::get_default_instance() {
    static otaPage page;
    return &page;
}
