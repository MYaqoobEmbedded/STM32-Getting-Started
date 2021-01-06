/*
 * flash.h
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

/**
 * Flash Unlock function
 */
void flash_unlock(void);

/**
 * Flash Lock function
 */
void flash_lock(void);

/**
 * Flash erase sector
 */
void flash_erase(uint32_t pageAddrs);

/**
 * Flash Write-16
 */
void flash_write16(uint32_t addrs, uint16_t *wr16, uint32_t len);

/**
 * Flash Read-16
 */
void flash_read16(uint32_t addrs, uint16_t *rd16, uint32_t len);

#endif /* INC_FLASH_H_ */
