/*
 * tim.h
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_TIM_H_
#define INC_TIM_H_

#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

/**
 * @brief TIM3 Configuration function
 */
bool tim_TIM3_config(uint16_t msPeriod);

/**
 * @brief TIM Output-Compare GPIO configuration
 */
void tim_TIM1_OC_GPIO_config(void);

/**
 * @brief TIM Output-Compare configuration
 */
bool tim_TIM1_OC_config(uint16_t msPeriod);

/**
 * @brief TIM Encoder GPIO configuration
 */
void tim_TIM4_ENCODER_GPIO_config(void);

/**
 * @brief TIM Encoder configuration
 */
bool tim_TIM4_ENCODER_config(void);

/**
 * @brief TIM3_CH1 - PWM Output GPIO
 */
void tim_TIM3_PWM_OUT_GPIO_config(void);

/**
 * @brief TIM3_CH1 - PWM Output
 */
bool tim_TIM3_PWM_OUT_config(void);

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

#endif /* INC_TIM_H_ */
