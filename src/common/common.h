#pragma once
#include <Arduino.h>
#include "pinouts.h"
#include "config.h"
#include "stm32f1_uid.h"

#define SerialDebug Serial

void debug_init(int * error);
#ifdef SYSTEM_DEBUG
#define DEBUG_INIT(x) debug_init(x)
#define DEBUG_INFO(x) SerialDebug.print(x)
#define DEBUG_INFO_LN(x) SerialDebug.println(x)
#else
#define DEBUG_INFO(x)
#define DEBUG_INIT(x)
#define DEBUG_INFO_LN(x)
#endif
