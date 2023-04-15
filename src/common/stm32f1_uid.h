#pragma once
#include <string>

/**
 * @brief Unique ID register address location
 */
#define ID_UNIQUE_ADDRESS       0x1FFFF7E8

/**
 * @brief  Get unique ID number in 32-bit format
 * @note   STM32F4xx has 96bits long unique ID, so 3 4-bytes values are available for read in 32-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 16 bits you want to read
 *               - Values between 0 and 2 are allowed
 * @retval Unique ID address
 */
#define STM32F1_ID_GetUnique32(x)   ((x >= 0 && x < 3) ? (*(uint32_t *) (ID_UNIQUE_ADDRESS + 4 * (x))) : 0)

std::string stm32f1_uid();