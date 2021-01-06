/*
 * rtc.h
 *
 *  Created on: Dec 9, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern const char* daysOfWkStr[7];

//Time
typedef struct ClockTime_TAG
{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hrs24;
} ClockTime_t;
//Date
typedef struct ClockDate_TAG
{
  uint8_t dayOfWk;
  uint8_t day;
  uint8_t month;
  uint16_t year;
} ClockDate_t;

typedef struct ClockAlarm_TAG
{
  ClockTime_t time;
} ClockAlarm_t;

//Callback function
typedef void (*AlarmEventCallback_t)(void);

/**
 * @brief RTC Configuration function
 */
bool rtc_config(void);

/**
 * @brief Set/Get Time
 */
void rtc_setTime(ClockTime_t *time);

/**
 * @brief Set/Get Date
 */
void rtc_setDate(ClockDate_t *date);

/**
 * @brief Get time & date, NOTE: We need to read time and date together
 */
void rtc_get_TimeDate(ClockTime_t *time, ClockDate_t *date);

/**
 * @brief Set Alarm
 */
void rtc_setAlarm(ClockAlarm_t *alarm, AlarmEventCallback_t cb);

/**
 * @brief Get Active Alarm
 */
void rtc_getAlarm(ClockAlarm_t *alarm);

/**
 * @brief Stop Alarm
 */
void rtc_stopAlarm(void);

/**
 * @brief alarm Callback
 */
void rtc_alarm_callback(void);

#endif /* INC_RTC_H_ */
