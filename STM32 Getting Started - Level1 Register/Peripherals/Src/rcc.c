/*
 * rcc.c
 *
 *  Created on: 3 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "rcc.h"

//Milisecond ticks
static __IO uint32_t msTicks=0;

/**
 * @brief HSE System clock configuration
 */
void rcc_HSE_config(void)
{
  /*
   * Configuration paramters
   *
   * HSE = 8MHz
   * PLL_M = 9
   * USB prescalar = 1.5
   * AHB prescalar = 1
   * Cortex precalar = 1
   * --> 72MHz System Clock
   *
   * APB1 prescalar = 2  --> 36, 72 MHz
   * APB2 prescalar = 1  --> 36, 36 MHz
   * ADC prescalr = 6    --> 12MHz
   */

  //PLL Configuration
  //PLL_MUL = 9
  RCC->CFGR &= ~RCC_CFGR_PLLMULL_Msk; //Clear
  RCC->CFGR |= (0x7UL << RCC_CFGR_PLLMULL_Pos);
  //USB Prescaler = 1.5
  RCC->CFGR &= ~(0x1UL << 22);

  //HSE Enable and Set as PLL source
  //Enable HSE Clock
  RCC->CR |= RCC_CR_HSEON;
  //Wait for HSE to be stable and ready to use
  while((RCC->CR & RCC_CR_HSERDY) == 0);
  //HSE as PLL source
  RCC->CFGR |= RCC_CFGR_PLLSRC;
  //Enable the PLL
  RCC->CR |= RCC_CR_PLLON;
  //Wait untill PLL is ready
  while((RCC->CR & RCC_CR_PLLRDY) == 0);

  //Flash pre-fetch enable and wait-state=2
  //0WS: 0-24MHz
  //1WS: 24-48MHz
  //2WS: 48-72MHz
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
  //Select PLL as main System Clock source
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_1;
  //Wait until PLL system source is active
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1);
  //AHB Prescaler = 1
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  //APB1 divider=2
  RCC->CFGR &= ~RCC_CFGR_PPRE1;
  RCC->CFGR |= RCC_CFGR_PPRE1_2;
  //APB2 divider=1
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  //ADC prescalar = 12
  RCC->CFGR &= ~RCC_CFGR_ADCPRE;
  RCC->CFGR |= RCC_CFGR_ADCPRE_1;
}

/**
 * @brief SysTick configuration
 */
void rcc_SysTick_config(uint32_t arr)
{
  SysTick->CTRL = 0;
  SysTick->LOAD = arr-1;
  NVIC_SetPriority(SysTick_IRQn, 0);

  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

////Startup file Vector Table callback for SysTick
//void SysTick_Handler(void)
//{
//  NVIC_ClearPendingIRQ(SysTick_IRQn);
//  msTicks++;
//}

/**
 * @brief Milisecond Delay and Get Ticks
 */
void rcc_msDelay(uint32_t ms)
{
  uint32_t startTicks = rcc_msGetTicks();
  while((rcc_msGetTicks() - startTicks) < ms);
}
uint32_t rcc_msGetTicks(void)
{
  return msTicks;
}

/**
 * @brief Inc Tick
 */
void msIncTick(void)
{
  msTicks++;
}
