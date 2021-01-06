/*
 * rcc.h
 *
 *  Created on: 3 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_RCC_H_
#define INC_RCC_H_

#include "main.h"

/**
 * @brief HSE System clock configuration
 */
void rcc_HSE_config(void);

/**
 * @brief SysTick configuration
 */
void rcc_SysTick_config(uint32_t arr);

/**
 * @brief Milisecond Delay and Get Ticks
 */
void rcc_msDelay(uint32_t ms);
uint32_t rcc_msGetTicks(void);

/**
 * @brief Inc Tick
 */
void msIncTick(void);

#endif /* INC_RCC_H_ */
