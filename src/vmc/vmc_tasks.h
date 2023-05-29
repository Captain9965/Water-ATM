#pragma once
#include "common/common.h"
#include "comms/comms.h"

void comms_task(void *pvParameters);
void main_task(void *pvParameters);
void input_task(void * prParameters);

BaseType_t * get_comms_taskhandle();
BaseType_t * get_main_taskhandle();
BaseType_t * get_input_task();