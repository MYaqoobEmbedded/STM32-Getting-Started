/*
 * rtc.c
 *
 *  Created on: Dec 9, 2020
 *      Author: Mohamed Yaqoob
 */

#include "rtc.h"

//RTC HAL Handle
RTC_HandleTypeDef hrtc;

const char* daysOfWkStr[7] = {
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
    "SUN"
};

static AlarmEventCallback_t ll_cb;

/**
 * @brief RTC Configuration function
 */
bool rtc_config(void)
{
  __HAL_RCC_RTC_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_BKP_CLK_ENABLE();

  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if((RCC->BDCR & RCC_BDCR_RTCEN) == 0)
  {
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
      return false;
    }
  }
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

  return true;
}

/**
 * @brief Set/Get Time
 */
void rtc_setTime(ClockTime_t *time)
{
  RTC_TimeTypeDef sTime = { 0 };
  sTime.Hours = time->hrs24;
  sTime.Minutes = time->minutes;
  sTime.Seconds = time->seconds;
  while (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK);
}

/**
 * @brief Set/Get Date
 */
void rtc_setDate(ClockDate_t *date)
{
  date->dayOfWk &= 0x7;
  RTC_DateTypeDef sDate = { 0 };
  sDate.Date    = date->day;
  sDate.Month   = date->month;
  sDate.Year    = date->year - 2000;
  sDate.WeekDay = date->dayOfWk;
  while(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK);
}

/**
 * @brief Get time & date, NOTE: We need to read time and date together
 */
void rtc_get_TimeDate(ClockTime_t *time, ClockDate_t *date)
{
  RTC_TimeTypeDef sTime = { 0 };
  while(HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK);
  time->hrs24   = sTime.Hours;
  time->minutes = sTime.Minutes;
  time->seconds = sTime.Seconds;

  RTC_DateTypeDef sDate = { 0 };
  while(HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK);
  date->day     = sDate.Date;
  date->month   = sDate.Month;
  date->year    = sDate.Year + 2000;
  date->dayOfWk = sDate.WeekDay;
}

/**
 * @brief Set Alarm
 */
void rtc_setAlarm(ClockAlarm_t *alarm, AlarmEventCallback_t cb)
{
  ll_cb = cb;
  RTC_AlarmTypeDef sAlarm = { 0 };
  sAlarm.AlarmTime.Hours = alarm->time.hrs24;
  sAlarm.AlarmTime.Minutes = alarm->time.minutes;
  sAlarm.AlarmTime.Seconds = alarm->time.seconds;
  sAlarm.Alarm = RTC_ALARM_A;
  while(HAL_RTC_SetAlarm_IT(&hrtc, &(sAlarm), RTC_FORMAT_BIN) != HAL_OK);
}

/**
 * @brief Get Active Alarm
 */
void rtc_getAlarm(ClockAlarm_t *alarm)
{
  RTC_AlarmTypeDef sAlarm = { 0 };
  while(HAL_RTC_GetAlarm(&hrtc, &(sAlarm), RTC_ALARM_A, RTC_FORMAT_BIN) != HAL_OK);
  alarm->time.hrs24   = sAlarm.AlarmTime.Hours;
  alarm->time.minutes = sAlarm.AlarmTime.Minutes;
  alarm->time.seconds = sAlarm.AlarmTime.Seconds;
}

/**
 * @brief Stop Alarm
 */
void rtc_stopAlarm(void)
{
  while(HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A) != HAL_OK);
}

/**
 * @brief alarm Callback
 */
void rtc_alarm_callback(void)
{
  //Call Alarm Callback
  ll_cb();
}
