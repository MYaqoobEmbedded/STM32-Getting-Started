/*
 * pwr.h
 *
 *  Created on: Dec 11, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_PWR_H_
#define INC_PWR_H_

#include "main.h"

/**
 * @brief Enter Sleep Mode
 */
void pwr_enterSleep(void);

/**
 * @brief Enter Stop Mode
 */
void pwr_enterStop(void);

/**
 * @brief Enter Standby Mode
 */
void pwr_enterStandby(void);

#endif /* INC_PWR_H_ */
