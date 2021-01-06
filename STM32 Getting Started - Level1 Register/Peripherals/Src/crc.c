/*
 * crc.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "crc.h"

/**
 * @brief CRC Enable
 */
void crc_enable(void)
{
  RCC->AHBENR |= RCC_AHBENR_CRCEN;
  //Reset
  crc_reset();
}

/**
 * @brief CRC Disable
 */
void crc_disable(void)
{
  RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
}

/**
 * @brief CRC Reset
 */
void crc_reset(void)
{
  CRC->CR = 0x01;
}

/**
 * @brief CRC Accumulate
 */
void crc_accumulate(uint32_t word32)
{
  CRC->DR = word32;
}

/**
 * @brief CRC Read
 */
uint32_t crc_read(void)
{
  return CRC->DR;
}

