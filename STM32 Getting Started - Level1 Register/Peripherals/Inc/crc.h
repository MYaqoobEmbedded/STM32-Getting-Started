/*
 * crc.h
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include "main.h"

/**
 * @brief CRC Enable
 */
void crc_enable(void);

/**
 * @brief CRC Disable
 */
void crc_disable(void);

/**
 * @brief CRC Reset
 */
void crc_reset(void);

/**
 * @brief CRC Accumulate
 */
void crc_accumulate(uint32_t word32);

/**
 * @brief CRC Read
 */
uint32_t crc_read(void);

#endif /* INC_CRC_H_ */
