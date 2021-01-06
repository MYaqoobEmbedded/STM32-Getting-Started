/*
 * exti.c
 *
 *  Created on: 5 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "exti.h"

void exti_buttonConfig(void)
{
  //Enable AFIO Clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  //PA0 as EXTI mux
  AFIO->EXTICR[0] &= ~(0xFUL << 0);
  //Unmask P0
  EXTI->IMR |= (1UL << 0);
  //Enable Rising edge
  EXTI->RTSR |= (1UL << 0);  //Rising edge=1
  EXTI->FTSR &= ~(1UL << 0); //Falling edge=0
  NVIC_SetPriority(EXTI0_IRQn, 5);
  NVIC_EnableIRQ(EXTI0_IRQn);
}
