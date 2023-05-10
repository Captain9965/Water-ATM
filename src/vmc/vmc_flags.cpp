#include "vmc_flags.h"

static uint32_t VMC_FLAGS = 0;

void set_vmc_flag(vmc_flags_t flag){
    VMC_FLAGS |= flag;
    return;
}

void clear_vmc_flag(vmc_flags_t flag){
    VMC_FLAGS &= ~(flag);
    return;
}

void vmc_flags_init(){
    VMC_FLAGS = 0;
    return;
}

bool check_vmc_flag(vmc_flags_t flag){
    return (VMC_FLAGS & flag);
}