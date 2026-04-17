#include "vmc_ota.h"
#include "../idle/vmc_idle.h"
#include "ota/ota.h"

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
