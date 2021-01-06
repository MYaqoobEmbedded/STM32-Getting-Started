/*
 * i2c.h
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "main.h"

#define __I2C_CLEAR_ADDRFLAG()    \
  do{                                           \
    __IO uint32_t tmpreg = 0x00U;               \
    tmpreg = I2C1->SR1;       \
    tmpreg = I2C1->SR2;       \
    (void)tmpreg;                             \
  } while(0)

/**
 * @brief I2C GPIO configuration
 */
void i2c_I2C1_gpioConfig(void);

/**
 * @brief I2C Peripheral configuration
 */
void i2c_I2C1_config(void);

/**
 * @brief I2C Check Address
 */
bool i2c_I2C1_isSlaveAddressExist(uint8_t addrs, uint32_t timeout);

/**
 * @brief I2C Transmit
 */
bool i2c_I2C1_masterTransmit(uint8_t addrs, uint8_t *pData, uint8_t len, uint32_t timeout);

/**
 * @brief I2C Receive
 */
bool i2c_I2C1_masterReceive(uint8_t addrs, uint8_t *pData, uint8_t len, uint32_t timeout);

#endif /* INC_I2C_H_ */
