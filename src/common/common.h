#pragma once
#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include "pinouts.h"
#include "config.h"
#include "stm32f1_uid.h"

/* debug utils */
#define SerialDebug Serial

void debug_init(int * error);
char * stack_debug();
#ifdef SYSTEM_DEBUG
#define DEBUG_INIT(x) debug_init(x)
#define DEBUG_INFO(x) SerialDebug.print(x)
#define DEBUG_INFO_LN(x) SerialDebug.println(x)
#define DEBUG_INFO_FMT(x, y) SerialDebug.print(x, y)
#else
#define DEBUG_INFO(x)
#define DEBUG_INIT(x)
#define DEBUG_INFO_LN(x)
#define DEBUG_INFO_FMT(x, y)
#endif

/* delay utils */
#define wait_ms(x) vTaskDelay(x/ portTICK_PERIOD_MS)
// #define wait_us(x) delayMicroseconds(x)

