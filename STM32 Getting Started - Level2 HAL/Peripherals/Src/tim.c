/*
 * tim.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "tim.h"

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

/**
 * @brief TIM3 Configuration function
 */
bool tim_TIM3_config(uint16_t msPeriod)
{
  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = (msPeriod*10)-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 7200-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    return false;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    return false;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    return false;
  }
  HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  return true;
}

/**
 * @brief TIM Output-Compare GPIO configuration
 */
void tim_TIM1_OC_GPIO_config(void)
{
  //GPIO Pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**TIM1 GPIO Configuration
  PB13     ------> TIM1_CH1N
  PB14     ------> TIM1_CH2N
  */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief TIM Output-Compare configuration
 */
bool tim_TIM1_OC_config(uint16_t msPeriod)
{
  //TIM - OC peripheral setup
  TIM_OC_InitTypeDef sConfigOC = {0};
  __HAL_RCC_TIM1_CLK_ENABLE();

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = (msPeriod*10)-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 7200-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    return false;
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    return false;
  }

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 3500-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    return false;
  }
  sConfigOC.Pulse = 7100-1;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    return false;
  }
  return true;
}

/**
 * @brief TIM Encoder GPIO configuration
 */
void tim_TIM4_ENCODER_GPIO_config(void)
{
  //GPIO Configuration
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**TIM4 GPIO Configuration
  PB6     ------> TIM4_CH1
  PB7     ------> TIM4_CH2
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief TIM Encoder configuration
 */
bool tim_TIM4_ENCODER_config(void)
{
  //TIM4 - Encoder Peripheral
  __HAL_RCC_TIM4_CLK_ENABLE();

  TIM_Encoder_InitTypeDef sConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65536-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 4;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 4;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    return false;
  }
  return true;
}

/**
 * @brief TIM3_CH1 - PWM Output GPIO
 */
void tim_TIM3_PWM_OUT_GPIO_config(void)
{
  //GPIO Configuration
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();
  /**TIM3 GPIO Configuration
  PB4     ------> TIM3_CH1 - R
  PB5     ------> TIM3_CH2 - G
  PB0     ------> TIM3_CH3 - B
  */
  GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  __HAL_AFIO_REMAP_TIM3_PARTIAL();
}

/**
 * @brief TIM3_CH1 - PWM Output
 */
bool tim_TIM3_PWM_OUT_config(void)
{
  //TIM3 - PWM Out peripheral configuration
  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1; //1MHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;//10KHz
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    return false;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    return false;
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    return false;
  }
  //Channel1
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    return false;
  }
  //Channel2
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    return false;
  }
  //Channel3
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    return false;
  }
  return true;
}

/**
 * @brief PWM LED Duty cycle
 */
void tim_PWM_setDutyCycle_CH1(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  uint16_t ccrValue = (uint16_t)(__HAL_TIM_GET_AUTORELOAD(&htim3) + 0.0f)*(duty_0_100/100.0f);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccrValue);
}

void tim_PWM_setDutyCycle_CH2(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  uint16_t ccrValue = (uint16_t)(__HAL_TIM_GET_AUTORELOAD(&htim3) + 0.0f)*(duty_0_100/100.0f);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, ccrValue);
}

void tim_PWM_setDutyCycle_CH3(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  uint16_t ccrValue = (uint16_t)(__HAL_TIM_GET_AUTORELOAD(&htim3) + 0.0f)*(duty_0_100/100.0f);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, ccrValue);
}

/**
 * @brief PWM LED RGB
 */
void tim_PWM_setRGB(uint8_t R, uint8_t G, uint8_t B)
{
  tim_PWM_setDutyCycle_CH1(100*(R/255.0f));
  tim_PWM_setDutyCycle_CH2(100*(G/255.0f));
  tim_PWM_setDutyCycle_CH3(100*(B/255.0f));
}
