#include "vmc.h"


void State::set_vmc_ctx(VMC* vmc){
    assert(vmc != nullptr);
    this->vmc = vmc;
    return;
}

State::~State(){};

int State::start(){
    started = true;
    DEBUG_INFO_LN("Default state starting..");
    return 0;
}

int State::run(){
    if(!started){
        start();
    }
    DEBUG_INFO_LN("Default state running..");
    return 0;
}