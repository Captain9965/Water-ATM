#include "vmc_ota.h"
#include "../idle/vmc_idle.h"
#include "ota/ota.h"
#include "ui/ui.h"

vmc_ota::vmc_ota() {
    id = "OTA";
}

int vmc_ota::start() {
    started = true;
    DEBUG_INFO_LN("[OTA] Entering OTA state — applying firmware");
    return 0;
}

int vmc_ota::run() {
    if (!started) start();

    OTAManager* ota = OTAManager::get_instance();

    if (!ota->is_ready()) {
        // begin() failed before we entered this state
        DEBUG_INFO_LN("[OTA] Not ready — returning to idle");
        this->vmc->set_state(vmc_idle::get_default_instance());
        return 0;
    }

    // Show "applying update" on both displays before the RAM trampoline
    // disables interrupts and locks out all library calls.
    get_display1()->clear();
    get_display1()->setCursor(1, 1);
    get_display1()->print("  Applying Update  ");
    get_display1()->setCursor(0, 2);
    get_display1()->print(" Do not switch off ");

    get_display2()->clear();
    get_display2()->setCursor(2, 1);
    get_display2()->print("  Please wait...  ");

    delay(200);   // give I2C time to flush before IRQs are disabled

    // CRC was already verified in begin().  Apply now — never returns.
    ota->apply();

    return 0;   // unreachable
}

int vmc_ota::stop() {
    started = false;
    return 0;
}

vmc_ota* vmc_ota::get_default_instance() {
    static vmc_ota instance;
    return &instance;
}
