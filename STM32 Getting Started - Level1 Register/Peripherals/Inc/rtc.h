/*
 * rtc.h
 *
 *  Created on: 12 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "main.h"

extern const char* daysOfWkString[7];

//RTC Time and Date
typedef struct RTC_TimeDate_TAG
{
  uint8_t hour;
  uint8_t min;
  uint8_t sec;

  uint8_t wday;
  uint8_t mday;
  uint8_t month;
  uint16_t year;
}RTC_TimeDate_t;

/**
 * @brief RTC LSE configuration
 */
void rtc_LSE_config(void);

/**
 * @brief RTC configuration
 */
void rtc_config(void);

/**
 * @brief RTC set Unix counter
 */
void rtc_setUnixCounter(uint32_t unix);

/**
 * @brief RTC get unix counter
 */
uint32_t rtc_getUnixCounter(void);

/**
 * @brief RTC Set Time & Date
 * @note: No need to set Week Day, it will be calculated
 */
void rtc_setTimeDate(RTC_TimeDate_t *pTimeDate);

/**
 * @brief RTC Get Time & Date
 * @note: You can read Weekday here.
 */
void rtc_getTimeDate(RTC_TimeDate_t *pTimeDate);

/**
 * @brief RTC alarm confoguration
 */
void rtc_alarmConfig(void);

/**
 * @brief RTC set alarm
 */
void rtc_setAlarm(RTC_TimeDate_t *pTimeDate);

/**
 * @brief RTC get alarm
 */
void rtc_getAlarm(RTC_TimeDate_t *pTimeDate);

/**
 * @Enable Backup registers write
 */
void rtc_backupRegisters_writeEnable(void);

/**
 * @brief Tamper pin enable
 */
void rtc_tamper_enable(void);

/**
 * @brief Tamper pin disable
 */
void rtc_tamper_disable(void);


#endif /* INC_RTC_H_ */
