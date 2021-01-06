/*
 * tim.h
 *
 *  Created on: 7 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_TIM_H_
#define INC_TIM_H_

#include "main.h"

/**
 * @brief TIM3 ADC Trigger timer configuration
 */
void tim_TIM3_config(void);

/**
 * @brief Delay timer - TIM2
 */
void tim_TIM2_delayConfig(void);

/**
 * @brief milli second delay with TIM2
 */
void tim_TIM2_msDelay(uint32_t ms);

/**
 * @brief Periodic TIM interrupt - TIM2
 */
void tim_TIM2_periodicConfig(uint32_t msPeriod);

/**
 * @brief TIM1_OC GPIO Config (LED1-PB13)
 */
void tim_TIM1_GPIO_config(void);

/**
 * @brief TIM1_OC Config
 */
void tim_TIM1_OC_config(void);

/**
 * @brief PWM LED GPIO configuration
 */
void tim_PWM_LED_GPIO_config(void);

/**
 * @brief PWM configuration
 */
void tim_PWM_LED_config(void);

/**
 * @brief PWM LED Duty cycle
 */
void tim_PWM_setDutyCycle_CH1(uint8_t duty_0_100);
void tim_PWM_setDutyCycle_CH2(uint8_t duty_0_100);
void tim_PWM_setDutyCycle_CH3(uint8_t duty_0_100);

/**
 * @brief PWM LED RGB
 */
void tim_PWM_setRGB(uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Encoder GPIO configuration
 */
void tim_Encoder_GPIO_config(void);

/**
 * @brief Encoder configuration
 */
void tim_Encoder_config(void);

/** SYS TIM-Base Functions **/
bool timBase_initTick(void);


#endif /* INC_TIM_H_ */
