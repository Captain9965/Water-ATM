#pragma once

// #define DUMP_AT_COMMANDS
#define SYSTEM_DEBUG
// #define MEM_DEBUG
#define ERROR_DEBUG
#define WARNING_DEBUG
#define INFO_DEBUG
#define SYSTEM_GSM_BAUDRATE         115200
#define SYSTEM_DEBUG_BAUDRATE       115200

/* Board variant — define exactly one:
 *   BOARD_VARIANT_A  →  display1=0x25, display2=0x26
 *   BOARD_VARIANT_B  →  display1=0x3D, display2=0x3E
 */
#define BOARD_VARIANT_A
// #define BOARD_VARIANT_B

#if defined(BOARD_VARIANT_A)
    #define DISPLAY1_I2C_ADDR   0x25
    #define DISPLAY2_I2C_ADDR   0x26
#elif defined(BOARD_VARIANT_B)
    #define DISPLAY1_I2C_ADDR   0x3D
    #define DISPLAY2_I2C_ADDR   0x3E
#else
    #error "No board variant defined. Define BOARD_VARIANT_A or BOARD_VARIANT_B in config.h"
#endif
