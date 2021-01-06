/*
 * gpio.h
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "main.h"

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

#endif /* INC_GPIO_H_ */
