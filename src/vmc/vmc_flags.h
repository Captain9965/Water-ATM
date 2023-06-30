#pragma once
#include "common/common.h"


typedef enum vmc_flags{
    VMC_NET_CONNECTED = 1 << 0,
    VMC_SD_CARD_OK = 1 << 1,
    VMC_MEMORY_ERROR = 1 << 2,
    VMC_STORAGE_ERROR = 1 << 3,
    VMC_BOOTING_DONE = 1 << 4


}vmc_flags_t;

void set_vmc_flag(vmc_flags_t flag);
void clear_vmc_flag(vmc_flags_t flag);
void vmc_flags_init();
bool check_vmc_flag(vmc_flags_t flag);