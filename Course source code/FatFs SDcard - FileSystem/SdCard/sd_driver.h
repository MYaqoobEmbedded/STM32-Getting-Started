/*
 * sd_driver.h
 *
 *  Created on: 22 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef SDCARD_SD_DRIVER_H_
#define SDCARD_SD_DRIVER_H_

#include "main.h"

/* Definitions for MMC/SDC command */
#define CMD0     (0x40+0)     /* GO_IDLE_STATE */
#define CMD1     (0x40+1)     /* SEND_OP_COND */
#define CMD8     (0x40+8)     /* SEND_IF_COND */
#define CMD9     (0x40+9)     /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

//SD Card State
typedef enum SD_State_TAG
{
  SD_State_OK=0,
  SD_State_Busy,
}SD_State_e;

#define STATUS_NOINIT    0x01  /* Drive not initialized */
#define STATUS_NODISK    0x02  /* No medium in the drive */
#define STATUS_PROTECT   0x04  /* Write protected */

typedef enum DiskStatus_TAG
{
  DiskStatus_NOINIT = 0x01,
  DiskStatus_NODISK = 0x02,
  DiskStatus_PROTECT = 0x04,
}DiskStatus_e;

//Low-Level functions
bool sd_driver_ll_getSectorCount(uint32_t *sectorCount);
bool sd_driver_ll_getSectorSize(uint32_t *sectorSize);
void sd_driver_ll_controlPower(bool stateOnOff);
bool sd_driver_ll_getPowerState(void);
bool sd_driver_ll_sync(void);
bool sd_driver_ll_getCSD(uint8_t *pBuf16Bytes);
bool sd_driver_ll_getCID(uint8_t *pBuf16Bytes);
bool sd_driver_ll_getOCR(uint8_t *pBuf4Bytes);


//High-Level functions
/**
 * @brief SD Card Initialise
 */
DiskStatus_e sd_driver_init(void);

/**
 * @brief SD Card Status
 */
SD_State_e sd_driver_getState(void);

/**
 * @brief SD Card Size info
 */
void sd_driver_getSizeInfo(uint32_t *pBlockSize, uint32_t *pBlockNbr);

/**
 * @brief Read Block
 */
bool sd_driver_readBlocks(uint8_t *pData, uint32_t readAddr, uint32_t numOfBlocks);

/**
 * @brief Write Block
 */
bool sd_driver_writeBlocks(uint8_t *pData, uint32_t writeAddr, uint32_t numOfBlocks);

#endif /* SDCARD_SD_DRIVER_H_ */
