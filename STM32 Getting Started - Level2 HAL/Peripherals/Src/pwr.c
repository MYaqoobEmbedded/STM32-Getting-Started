/*
 * pwr.c
 *
 *  Created on: Dec 11, 2020
 *      Author: Mohamed Yaqoob
 */

#include "pwr.h"
#include "rcc.h"

/**
 * @brief Enter Sleep Mode
 */
void pwr_enterSleep(void)
{
  //Enter Sleep Mode
  HAL_SuspendTick();
  HAL_PWR_EnableSleepOnExit();
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  //On Exit from Sleep mode, resume SysTick operation if disabled
  HAL_ResumeTick();
}

/**
 * @brief Enter Stop Mode
 */
void pwr_enterStop(void)
{
  HAL_SuspendTick();
  HAL_PWR_DisableSleepOnExit();
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  //HSE configure
  rcc_systemClockConfig();
  //On Exit from Sleep mode, resume SysTick operation if disabled
  HAL_ResumeTick();
}

/**
 * @brief Enter Standby Mode
 */
void pwr_enterStandby(void)
{
  /* Disable all used wakeup sources: PWR_WAKEUP_PIN1 */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  /* Clear all related wakeup flags*/
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  /* Enable WakeUp Pin PWR_WAKEUP_PIN1 connected to PA.00 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  /* Enter the Standby mode */
  HAL_PWR_EnterSTANDBYMode();
}
