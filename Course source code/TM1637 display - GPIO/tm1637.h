/*
 * tm1637.h
 *
 *  Created on: 26 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef DISPLAY_TM1637_H_
#define DISPLAY_TM1637_H_

#include "main.h"

/**
 * @brief initialisation
 */
void tm1637_init(GPIO_TypeDef *port, uint16_t clkPin, uint16_t dioPin);

/**
 * @brief write display data
 */
void tm1637_writeDigits(uint8_t digitsData[4], bool colomn);

/**
 * @brief Set Brightness
 */
void tm1637_setBrightness(uint8_t value_0_7);

/**
 * @brief Counter display
 */
void tm1637_setCounter(uint16_t counter);

/**
 * @brief Set Clock
 * @description: Calling this function causes Column to toggle
 */
void tm1637_setClock(uint8_t hrs, uint8_t mins);

#endif /* DISPLAY_TM1637_H_ */
