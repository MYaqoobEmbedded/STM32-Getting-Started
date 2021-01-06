/*
 * flash.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "flash.h"

/**
 * Flash Unlock function
 */
void flash_unlock(void)
{
  if(FLASH->CR & FLASH_CR_LOCK)
  {
    FLASH->KEYR = 0x45670123U;
    FLASH->KEYR = 0xCDEF89ABU;
  }
}

/**
 * Flash Lock function
 */
void flash_lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}

/**
 * Flash erase sector
 */
void flash_erase(uint32_t pageAddrs)
{
  //Wait for BSY flag to be idle
  while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
  //Enable PER bit for Page Erase
  FLASH->CR |= FLASH_CR_PER;
  //Page Address to Erase
  FLASH->AR = pageAddrs;
  //Star Erase
  FLASH->CR |= FLASH_CR_STRT;
  //Wait for BSY flag to be idle
  while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
  //Disable PER bit
  CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
}

/**
 * Flash Write-16
 */
void flash_write16(uint32_t addrs, uint16_t *wr16, uint32_t len)
{
  //Write data in 16-bit at a time
  uint32_t tempAddrs = addrs;
  for(uint32_t i=0; i<len; i++)
  {
    //Set PG bit
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t*) tempAddrs = wr16[i];
    tempAddrs+=2;
  }
  //Wait for BSY flag to be idle
  while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
  //Disable PG bit
  CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

/**
 * Flash Read-16
 */
void flash_read16(uint32_t addrs, uint16_t *rd16, uint32_t len)
{
  //Read data can be accessed straight away
  uint32_t tempAddrs = addrs;
  for(uint32_t i=0; i<len; i++)
  {
    rd16[i] = *(__IO uint16_t*) tempAddrs;
    tempAddrs+=2;
  }
}

