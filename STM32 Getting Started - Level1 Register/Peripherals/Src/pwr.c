/*
 * pwr.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "pwr.h"

/**
 * @brief Enter Sleep Mode
 */
void pwr_enterSleep(void)
{
  //Disable SysTick Interrupt (To avoid waking MCU up)
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  //Clear SleepOnExit (if Set, serves interrupt and back to sleep)
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
  //Clear SleepDEEP bit in SCR register
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
  //Call Low Power mode, interrupt wakeup instruction
  __WFI();
  //Resume SysTick Interrupt
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief Enter Stop Mode
 */
void pwr_enterStop(void)
{
  //Disable SysTick Interrupt (To avoid waking MCU up)
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  //Clear SleepOnExit (if Set, serves interrupt and back to sleep)
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
  //Set SleepDEEP bit in SCR register
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
  //Clear PDDS bit in the STM32 PWR_CR register
  CLEAR_BIT(PWR->CR, ((uint32_t)PWR_CR_PDDS_Msk));
  //Call Low Power mode, interrupt wakeup instruction
  __WFI();
  //Resume SysTick Interrupt
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief Enter Standby Mode
 */
void pwr_enterStandby(void)
{
  //Enable WKUP bit in the PWR_CSR to allow wakeup through button
  SET_BIT(PWR->CSR, PWR_CSR_EWUP_Msk);
  //Disable SysTick Interrupt (To avoid waking MCU up)
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  //Set SleepDEEP bit in SCR register
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
  //Set PDDS bit in the STM32 PWR_CR register
  SET_BIT(PWR->CR, ((uint32_t)PWR_CR_PDDS_Msk));
  //Clear SleepOnExit (if Set, serves interrupt and back to sleep)
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
  //Clear Standby flag
  SET_BIT(PWR->CR, ((uint32_t)PWR_CR_CSBF_Msk));
  //Clear WUF bit in the PWR_CSR register
  SET_BIT(PWR->CR, ((uint32_t)PWR_CR_CWUF_Msk));
  //Call Low Power mode, interrupt wakeup instruction
  __WFI();
  //System will wakeup with an effective Reset, so no need
  //to do any re-enable
}

