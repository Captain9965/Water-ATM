#pragma once
#include "common/common.h"
#define WATCHDOG_TIMEOUT_TICKS pdMS_TO_TICKS(3000)
#define WATCHDOG_CHECK_PERIOD_TICKS pdMS_TO_TICKS(100)


typedef enum {
    TASK_IDX_COMMS = 0,
    TASK_IDX_MAIN,
    TASK_IDX_INPUT,
    NUM_MONITORED_TASKS 
} MonitoredTaskIndex_t;


static TickType_t xTaskLastCheckIn[NUM_MONITORED_TASKS];