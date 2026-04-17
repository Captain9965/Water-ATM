#pragma once
#include "vmc/vmc.h"

class vmc_ota : public State {
public:
    vmc_ota();
    int start() override;
    int stop()  override;
    int run()   override;
    static vmc_ota* get_default_instance();
private:
    uint32_t _tick_time = 0;
};
