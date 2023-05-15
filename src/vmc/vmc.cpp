#include "vmc.h"
#include "assert.h"
VMC::~VMC(){
    delete vmc_state;
}

int VMC::start(){
    this->vmc_state->start();
    return 0;
}

int VMC::stop(){
    this->vmc_state->stop();
    return 0;
}

int VMC::run(){
    this->vmc_state->run();
    return 0;
}

void VMC::set_state(State *state){
    assert(state != nullptr);
    DEBUG_INFO("Transitioning state to ");
    DEBUG_INFO_LN(state->id);
    state->started = false;
    this->vmc_state = state;
    this->vmc_state->set_vmc_ctx(this);
    return;
}

VMC* VMC::get_default_instance(){
    static VMC vmc;
    return &vmc;
}

State* VMC::get_state(){
    return vmc_state;
}