/*
 * gpio.c
 *
 *  Created on: 3 Nov 2020
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
  //Enable Port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  //output mode, low-speed
  GPIOB->CRH &= ~GPIO_CRH_CNF13;
  GPIOB->CRH &= ~GPIO_CRH_CNF14;
  //Push-pull
  GPIOB->CRH &= ~GPIO_CRH_MODE13;
  GPIOB->CRH |= GPIO_CRH_MODE13_1;
  GPIOB->CRH &= ~GPIO_CRH_MODE14;
  GPIOB->CRH |= GPIO_CRH_MODE14_1;
}

/**
 * @brief LEDs write/toggle (Green-Red LEDs)
 */
void gpio_LED_writeGreen(bool state)
{
  if(state)
  {
    GPIOB->ODR |= (1UL << 13);
  }
  else
  {
    GPIOB->ODR &= ~(1UL << 13);
  }
}

void gpio_LED_toggleGreen(void)
{
  GPIOB->ODR ^= (1UL << 13);
}

void gpio_LED_writeRed(bool state)
{
  if(state)
  {
    GPIOB->ODR |= (1UL << 14);
  }
  else
  {
    GPIOB->ODR &= ~(1UL << 14);
  }
}

void gpio_LED_toggleRed(void)
{
  GPIOB->ODR ^= (1UL << 14);
}

/**
 * @brief Button configuration
 */
void gpio_PB_config(void)
{
  //PA0 Push button
  //Enable port A clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  //Input mode
  GPIOA->CRL &= ~GPIO_CRL_MODE0;
  //Input
  GPIOA->CRL &= ~GPIO_CRL_CNF0;
  GPIOA->CRL |= GPIO_CRL_CNF0_1;
}
bool gpio_PB_read(void)
{
  return (bool)(GPIOA->IDR & GPIO_IDR_IDR0);
}

/**
 * @brief Switches 2P
 */
void gpio_SW_config(void)
{
  //PA8 Push button
  //Enable port A clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  //Input mode
  GPIOA->CRH &= ~GPIO_CRH_MODE8;
  //Input
  GPIOA->CRH &= ~GPIO_CRH_CNF8;
  GPIOA->CRH |= GPIO_CRH_CNF8_1;

  //PA15 Push button
  //Input mode
  GPIOA->CRH &= ~GPIO_CRH_MODE15;
  //Input
  GPIOA->CRH &= ~GPIO_CRH_CNF15;
  GPIOA->CRH |= GPIO_CRH_CNF15_1;
  //apply pull-down
  GPIOA->ODR &= ~(1UL << 15);

  //Remap PA15 to disable JTDI
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_1);
}
bool gpio_SW1_read(void)
{
  return (bool)(GPIOA->IDR & GPIO_IDR_IDR8);
}
bool gpio_SW2_read(void)
{
  return (bool)(GPIOA->IDR & GPIO_IDR_IDR15);
}

/**
 * @brief PC13 GPIO - LED and Tamper
 */
void gpio_PC13_config(void)
{
  //Enable Port C clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  //input mode, low-speed
  GPIOC->CRH &= ~GPIO_CRH_CNF13;
  //Input pull-down
  GPIOC->CRH &= ~GPIO_CRH_MODE13;
  GPIOC->CRH |= GPIO_CRH_MODE13_0;
  GPIOC->ODR &= ~(1UL << 13);
}
