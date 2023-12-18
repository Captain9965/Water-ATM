#pragma once
#include "common/common.h"
#include "vmc_tasks.h"
#include "vmc_errors.h"
class VMC;
/* Base state class */
class State{
    protected:
        VMC* vmc = nullptr;
    public:
        virtual ~State();

        void set_vmc_ctx(VMC* vmc);
        virtual int start();
        virtual int stop();
        virtual int run();
        const char* id = nullptr;
        bool started = false;
};
/* vmc class */
class VMC{
    protected:
        State *vmc_state =  nullptr;
    public:
        ~VMC();
        virtual int start();
        virtual int stop();
        virtual int run();
        virtual void set_state(State *state);
        virtual State* get_state();
        static VMC* get_default_instance();
        // get_state()?
        vmc_error_t error;
};