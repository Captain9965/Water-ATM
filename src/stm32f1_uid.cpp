#include "stm32f1_uid.h"
#include <Arduino.h>

std::string stm32f1_uid()
{
    char buff[200];
    int s = sprintf(buff, "%lx-%lx-%lx",
                    STM32F1_ID_GetUnique32(0),    /* LSB */
                    STM32F1_ID_GetUnique32(1),
                    STM32F1_ID_GetUnique32(2)    /* MSB */
                   );
    return std::string(buff, s);
}