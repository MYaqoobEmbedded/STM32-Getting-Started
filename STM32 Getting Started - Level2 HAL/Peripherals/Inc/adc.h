/*
 * adc.h
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"

extern ADC_HandleTypeDef adc1Handle;

typedef enum ADC_SingleSelect_TAG
{
  ADC_SingleSelect_Potentiometer=1,
  ADC_SingleSelect_Joystick_x,
  ADC_SingleSelect_Joystick_y_Mic,
}ADC_SingleSelect_e;

/**
 * @brief ADC channels GPIO configuration
 */
void adc_GPIO_config(void);

/**
 * @brief ADC single channel configuration
 */
bool adc_singleConfig(ADC_SingleSelect_e channel);

/**
 * @brief ADC Interrupt
 */
void adc_interruptConfig(void);

/**
 * @brief ADC Multi-Channel Config
 */
bool adc_multiChannelConfig(void);

/**
 * @brief ADC Multi-Channel DMA Configuration
 */
void adc_dmaConfig(void);

/**
 * @brief ADC Analog Watchdog configuration
 */
bool adc_awdgConfig(ADC_SingleSelect_e channel);

/**
 * @brief ADC Injected Channel configuration
 */
bool adc_injectedConfig(void);

#endif /* INC_ADC_H_ */
