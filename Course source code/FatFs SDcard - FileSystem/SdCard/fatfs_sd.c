/*
 * fatfs_sd.c
 *
 *  Created on: Jul 2, 2020
 *      Author: meh
 */


#define TRUE  1
#define FALSE 0
#define bool BYTE

#include "diskio.h"
#include "fatfs_sd.h"
#include "main.h"
#include "sd_driver.h"

static volatile DSTATUS Stat = STA_NOINIT;

/**
 * FATFS Interface functions, managed by USER_DISKIO
 */

DSTATUS SD_disk_initialize(BYTE drv)
{
  return (DSTATUS)sd_driver_init();
}

DSTATUS SD_disk_status(BYTE drv)
{
  if(sd_driver_getState() == SD_State_OK)
  {
    Stat &= ~STA_NOINIT;
  }
  return Stat;
}

DRESULT SD_disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  if(sd_driver_readBlocks((uint8_t*)buff, (uint32_t)(sector), count))
  {
    return RES_OK;
  }
  return RES_ERROR;
}

#if _READONLY == 0
DRESULT SD_disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
  if(sd_driver_writeBlocks((uint8_t*)buff, (uint32_t)(sector), count))
  {
    return RES_OK;
  }
  return RES_ERROR;
}
#endif /* _READONLY */

DRESULT SD_disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
  BYTE *ptr = buff;

  /* USER CODE BEGIN IOCTL */
  DRESULT res = RES_ERROR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (ctrl)
  {
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
  {
    uint32_t sectorCount;
    if(sd_driver_ll_getSectorCount(&sectorCount))
    {
      *(DWORD*)buff = sectorCount;
      res = RES_OK;
    }
  }
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
  {
    uint32_t sectorSize;
    if(sd_driver_ll_getSectorSize(&sectorSize))
    {
      *(DWORD*)buff = sectorSize;
      res = RES_OK;
    }
  }
    break;
  case CTRL_SYNC:
    if(sd_driver_ll_sync())
    {
      res = RES_OK;
    }
    break;

  case MMC_GET_CSD:
    if(sd_driver_ll_getCSD((uint8_t *)buff))
    {
      res = RES_OK;
    }
    break;

  case MMC_GET_CID:
    if(sd_driver_ll_getCID((uint8_t *)buff))
    {
      res = RES_OK;
    }
    break;

  case MMC_GET_OCR:
    if(sd_driver_ll_getOCR((uint8_t *)buff))
    {
      res = RES_OK;
    }
    break;

  case CTRL_POWER:
  {
    switch (*ptr)
    {
    case 0:
      sd_driver_ll_controlPower(0);
      res = RES_OK;
      break;
    case 1:
      sd_driver_ll_controlPower(1);
      res = RES_OK;
      break;
    case 2:
      *(ptr + 1) = (BYTE) sd_driver_ll_getPowerState();
      res = RES_OK;             /* Power Check */
      break;
    default:
      res = RES_PARERR;
      break;
    }
  }
  break;

  default:
    res = RES_PARERR;
  }

  return res;
}
