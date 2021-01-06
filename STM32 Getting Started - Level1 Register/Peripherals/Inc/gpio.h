/*
 * gpio.h
 *
 *  Created on: 3 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "main.h"

//Bit-banding (PB13 - PB14)
#define ODR_OFFSET      (0x0C)
#define GPIOB_ODR_ADDR  (GPIOB_BASE - PERIPH_BASE + ODR_OFFSET)

#define PB13_BB_ADDR     (PERIPH_BB_BASE + GPIOB_ODR_ADDR*32 + 13*4)
#define PB14_BB_ADDR     (PERIPH_BB_BASE + GPIOB_ODR_ADDR*32 + 14*4)

#define LED_GREEN_BB    *(uint32_t *)(PB13_BB_ADDR)
#define LED_RED_BB      *(uint32_t *)(PB14_BB_ADDR)

//Push button Bit-banding
#define IDR_OFFSET      (0x08)
#define GPIOA_ODR_ADDR  (GPIOA_BASE - PERIPH_BASE + IDR_OFFSET)
#define PA0_BB_ADDR      (PERIPH_BB_BASE + GPIOA_ODR_ADDR*32 + 0*4)

#define BUTTON_BB       *(uint32_t *)(PA0_BB_ADDR)

/**
 * @brief LEDs GPIO Configuration
 */
void gpio_LED_config(void);

/**
 * @brief LEDs write/toggle (Green-Red LEDs)
 */
void gpio_LED_writeGreen(bool state);
void gpio_LED_toggleGreen(void);
void gpio_LED_writeRed(bool state);
void gpio_LED_toggleRed(void);

/**
 * @brief USER - Button
 */
void gpio_PB_config(void);
bool gpio_PB_read(void);

/**
 * @brief Switches 2P
 */
void gpio_SW_config(void);
bool gpio_SW1_read(void);
bool gpio_SW2_read(void);

/**
 * @brief PC13 GPIO - LED and Tamper
 */
void gpio_PC13_config(void);

#endif /* INC_GPIO_H_ */
