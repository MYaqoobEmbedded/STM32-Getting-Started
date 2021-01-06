/*
 * i2c.h
 *
 *  Created on: Dec 10, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief I2C1 GPIO Configuration
 */
void i2c_GPIO_config(void);

/**
 * @brief I2C1 Peripheral Configuration
 */
bool i2c_config(void);

#endif /* INC_I2C_H_ */
