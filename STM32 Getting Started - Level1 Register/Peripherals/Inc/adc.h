/*
 * adc.h
 *
 *  Created on: 7 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"

typedef enum ADC_SingleSelect_TAG
{
  ADC_SingleSelect_Potentiometer=1,
  ADC_SingleSelect_Joystick_x,
  ADC_SingleSelect_Joystick_y_Mic,
}ADC_SingleSelect_e;

/**
 * @brief ADC Calibrate
 */
void adc_calibrate(void);

/**
 * @brief ADC Start
 */
void adc_start(void);

/**
 * @brief ADC Stop
 */
void adc_stop(void);

/**
 * @brief ADC channels GPIO configuration
 */
void adc_GPIO_config(void);

/**
 * @brief ADC single channel configuration
 */
void adc_singleConfig(ADC_SingleSelect_e channel);

/**
 * @brief Poll for end of conversion
 */
bool adc_pollForEndOfConversion(uint32_t msTimeout);

/**
 * @brief Read value
 */
uint16_t adc_readValue(void);

/**
 * @brief ADC Enable Interrupt
 */
void adc_enableIT(void);
void adc_disableIT(void);

/**
 * @brief ADC Multi-channel
 */
void adc_multiChannelConfig(void);
void adc_multiChannel_DMA_config(uint16_t *pAdcBuffer);

/**
 * @brief ADC analog watchdog configure
 */
void adc_AWD_Config(uint8_t channel, uint16_t lowThreshold, uint16_t highThreshold);

/**
 * @brief ADC Injected configure
 */
void adc_INJ_Configure(uint8_t channel);

/**
 * @brief ADC Injected Start
 */
void adc_INJ_Start(void);

/**
 * @brief ADC poll for end of conversion
 */
bool adc_INJ_pollForConversion(uint32_t timeout);


#endif /* INC_ADC_H_ */
