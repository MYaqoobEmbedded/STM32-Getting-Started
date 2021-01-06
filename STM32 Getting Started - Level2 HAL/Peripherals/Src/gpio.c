/*
 * gpio.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "gpio.h"

/**
 * @brief LEDs GPIO Configuration
 */
void gpio_LED_config(void)
{
  //Green LED: PB13
  //Red LED: PB14
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief LEDs write/toggle (Green-Red LEDs)
 */
void gpio_LED_writeGreen(bool state)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (GPIO_PinState)state);
}

void gpio_LED_toggleGreen(void)
{
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
}

void gpio_LED_writeRed(bool state)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (GPIO_PinState)state);
}

void gpio_LED_toggleRed(void)
{
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
}

/**
 * @brief USER - Button
 */
void gpio_PB_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  //PA0
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

bool gpio_PB_read(void)
{
  return (bool)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
}

/**
 * @brief Switches 2P
 */
void gpio_SW_config(void)
{
  //PA8, PA15
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  //PA15 JTAG to disable
  __HAL_AFIO_REMAP_SWJ_DISABLE();
}

bool gpio_SW1_read(void)
{
  return (bool)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
}

bool gpio_SW2_read(void)
{
  return (bool)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
}
