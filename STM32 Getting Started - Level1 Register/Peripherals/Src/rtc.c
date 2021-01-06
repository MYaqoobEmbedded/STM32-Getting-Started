/*
 * rtc.c
 *
 *  Created on: 12 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "rtc.h"
#include "stdio.h"
#include <time.h>

const char* daysOfWkString[7] = {
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
    "SUN"
};

/**
 * @brief RTC LSE configuration
 */
void rtc_LSE_config(void)
{
  //Enable power clock
  RCC->APB1ENR |= (1UL << 28);
  __IO uint32_t tempRead = 0x00U;
  tempRead = RCC->APB1ENR;
  //Enable write access to backup domain
  PWR->CR |= (1UL << 8);
  while(!(PWR->CR & (1UL << 8)));
  //Check if LSE is already source
  uint32_t RTC_bdcr = RCC->BDCR;
  if((RTC_bdcr & (0x01 << 8)) && (RTC_bdcr & ~(0x01 << 9)))
  {
    printf("LSE is already selected!\n");
    return;
  }
  //Reset Backup domain (to allow us change clock source)
  tempRead = 0x00U;
  tempRead = (RCC->BDCR & ~(3UL << 8)); // Clear Clock Select bits
  //Force
  RCC->BDCR = (1UL << 16);
  //Release
  RCC->BDCR = 0;
  //Restore value
  RCC->BDCR = tempRead;
  //Enable LSE and wait for ready
  RCC->BDCR |= (1UL << 0);
  while(!(RCC->BDCR & (1UL << 1)));
  //Select LSE as source
  RCC->BDCR &= ~(3UL << 8);
  RCC->BDCR |= (1UL << 8);
  //Enable RTC
  RCC->BDCR |= (1UL << 15);
}

/**
 * @brief RTC configuration
 */
void rtc_config(void)
{
  while(!(RTC->CRL & (1UL << 5)));
  //Enter configuration mode
  RTC->CRL |= (1UL << 4);
  //Set prescalar load value to 0x7FFF for 32.768KHz LSE
  RTC->PRLL = 0x7FFF;
  RTC->PRLH = 0;
  //Exit initialisation mode
  RTC->CRL &= ~(1UL << 4);
  while(!(RTC->CRL & (1UL << 5)));
}

/**
 * @brief RTC set Unix counter
 */
void rtc_setUnixCounter(uint32_t unix)
{
  while(!(RTC->CRL & (1UL << 5)));
  //Enter configuration mode
  RTC->CRL |= (1UL << 4);
  //Set Unix counter
  RTC->CNTL = unix&0xFFFF;
  RTC->CNTH = (unix>>16)&0xFFFF;
  //Exit initialisation mode
  RTC->CRL &= ~(1UL << 4);
  while(!(RTC->CRL & (1UL << 5)));
}

/**
 * @brief RTC get unix counter
 */
uint32_t rtc_getUnixCounter(void)
{
  return (uint32_t)(RTC->CNTL + (RTC->CNTH << 16));
}

/**
 * @brief RTC Set Time & Date
 * @note: No need to set Week Day, it will be calculated
 */
void rtc_setTimeDate(RTC_TimeDate_t *pTimeDate)
{
  struct tm myTime;
  struct tm *timeDef=&myTime;
  time_t timeSecs;
  timeDef->tm_hour = pTimeDate->hour;
  timeDef->tm_min = pTimeDate->min;
  timeDef->tm_sec = pTimeDate->sec;
  timeDef->tm_mday = pTimeDate->mday;
  timeDef->tm_mon = pTimeDate->month-1;
  timeDef->tm_year = pTimeDate->year-1900;
  timeSecs = mktime(timeDef);
  rtc_setUnixCounter((uint32_t)timeSecs);
}

/**
 * @brief RTC Get Time & Date
 * @note: You can read Weekday here.
 */
void rtc_getTimeDate(RTC_TimeDate_t *pTimeDate)
{
  struct tm myTime;
  struct tm *timeDef=&myTime;
  time_t timeSecs;
  timeSecs = (time_t)rtc_getUnixCounter();
  timeDef = gmtime(&timeSecs);
  pTimeDate->hour   = timeDef->tm_hour;
  pTimeDate->min    = timeDef->tm_min;
  pTimeDate->sec    = timeDef->tm_sec;
  pTimeDate->mday   = timeDef->tm_mday;
  pTimeDate->month  = timeDef->tm_mon+1;
  pTimeDate->year   = timeDef->tm_year+1900;
  pTimeDate->wday   = timeDef->tm_wday;
}

/**
 * @brief RTC alarm confoguration
 */
void rtc_alarmConfig(void)
{
  //Enable RTC Alarm Interrupt
  NVIC_SetPriority(RTC_Alarm_IRQn, 5);
  NVIC_EnableIRQ(RTC_Alarm_IRQn);
  //Enable RTC Alarm EXTI
  EXTI->PR = EXTI_PR_PR17;
  EXTI->IMR = (1UL << 17);
  //Rising Edge
  EXTI->RTSR = (1UL << 17);
}

/**
 * @brief RTC set alarm
 */
void rtc_setAlarm(RTC_TimeDate_t *pTimeDate)
{
  struct tm myTime;
  struct tm *timeDef=&myTime;
  time_t timeSecs;
  timeDef->tm_hour = pTimeDate->hour;
  timeDef->tm_min = pTimeDate->min;
  timeDef->tm_sec = pTimeDate->sec;
  timeDef->tm_mday = pTimeDate->mday;
  timeDef->tm_mon = pTimeDate->month-1;
  timeDef->tm_year = pTimeDate->year-1900;
  timeSecs = mktime(timeDef);

  while(!(RTC->CRL & (1UL << 5)));
  //Enter configuration mode
  RTC->CRL |= (1UL << 4);
  //Set Unix alarm
  RTC->ALRL = timeSecs&0xFFFF;
  RTC->ALRH = (timeSecs>>16)&0xFFFF;
  //Exit initialisation mode
  RTC->CRL &= ~(1UL << 4);
  while(!(RTC->CRL & (1UL << 5)));
}

/**
 * @brief RTC get alarm
 */
void rtc_getAlarm(RTC_TimeDate_t *pTimeDate)
{
  struct tm myTime;
  struct tm *timeDef=&myTime;
  time_t timeSecs;
  timeSecs = (time_t)((RTC->ALRL) + (RTC->ALRH << 16));
  timeDef = gmtime(&timeSecs);
  pTimeDate->hour   = timeDef->tm_hour;
  pTimeDate->min    = timeDef->tm_min;
  pTimeDate->sec    = timeDef->tm_sec;
  pTimeDate->mday   = timeDef->tm_mday;
  pTimeDate->month  = timeDef->tm_mon+1;
  pTimeDate->year   = timeDef->tm_year+1900;
  pTimeDate->wday   = timeDef->tm_wday;
}

/**
 * @Enable Backup registers write
 */
void rtc_backupRegisters_writeEnable(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
  PWR->CR |= PWR_CR_DBP;
}

/**
 * @brief Tamper pin enable
 */
void rtc_tamper_enable(void)
{
  //Enable Tamper
  BKP->CR = (1UL << 0);
  //Enable Tamper Interrupt
  BKP->CSR |= (1UL << 2);
  NVIC_EnableIRQ(TAMPER_IRQn);
  //Clear Tamper interrupt
  BKP->CSR |= (3UL << 0);
}
/**
 * @brief Tamper pin disable
 */
void rtc_tamper_disable(void)
{
  //disable Tamper
  BKP->CR &= ~(1UL << 0);
  //Disable Tamper Interrupt
  BKP->CSR &= ~(1UL << 2);
  //Clear Tamper interrupt
  BKP->CSR |= (3UL << 0);
  NVIC_DisableIRQ(TAMPER_IRQn);
}

