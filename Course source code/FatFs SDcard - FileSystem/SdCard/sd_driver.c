/*
 * sd_driver.c
 *
 *  Created on: 22 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#include "sd_driver.h"
#include "rcc.h"
#include "spi.h"
#include "gpio.h"

#define SPI_TIMEOUT 1000

static volatile uint8_t Stat = STATUS_NOINIT;
static uint8_t CardType;                    /* Card Type, 0:MMC, 1:SDC */
static uint8_t PowerFlag = 0;               /* SD Card Power Status */
//ms Delay and Ticks Macros
#define MS_DELAY(ms) rcc_msDelay(ms)
#define MS_TICKS() rcc_msGetTicks()

//SPI Chip Select
static void SELECT(void)
{
  spi_cs_sd_write(0);
}

//SPI Chip Deselect
static void DESELECT(void)
{
  spi_cs_sd_write(1);
}

//SPI Transmit uint8_t
static void SPI_TxByte(uint8_t data)
{
  spi_transmit(&data, 1, SPI_TIMEOUT);
}

//SPI Receive uint8_t
static uint8_t SPI_RxByte(void)
{
  uint8_t data=0, dummy = 0xFF;;

  spi_transmitReceive(&dummy, &data, 1, SPI_TIMEOUT);
  return data;
}

//Receive by pointer
static void SPI_RxBytePtr(uint8_t *buff)
{
  *buff = SPI_RxByte();
}

//SD Card Read State implementation
static uint8_t SD_ReadyWait(void)
{
  uint8_t res;

  uint32_t startTicks = MS_TICKS();
  SPI_RxByte();

  do
  {
    res = SPI_RxByte();
  } while ((res != 0xFF) && ((MS_TICKS() - startTicks) < 500));

  return res;
}

//Power ON SD Card
static void SD_PowerOn(void)
{
  uint8_t cmd_arg[6];
  uint32_t Count = 0x1FFF;

  DESELECT();

  for(int i = 0; i < 10; i++)
  {
    SPI_TxByte(0xFF);
  }

  SELECT();

  //GO IDLE STATE
  cmd_arg[0] = (CMD0 | 0x40);
  cmd_arg[1] = 0;
  cmd_arg[2] = 0;
  cmd_arg[3] = 0;
  cmd_arg[4] = 0;
  cmd_arg[5] = 0x95;

  //Send Command Argument
  for (int i = 0; i < 6; i++)
  {
    SPI_TxByte(cmd_arg[i]);
  }

  //Wait for Response
  while ((SPI_RxByte() != 0x01) && Count)
  {
    Count--;
  }

  DESELECT();
  SPI_TxByte(0XFF);

  PowerFlag = 1;
}

//Power Off
static void SD_PowerOff(void)
{
  PowerFlag = 0;
}

//Return power state
static uint8_t SD_CheckPower(void)
{
  /*  0=off, 1=on */
  return PowerFlag;
}

//Read Block, 512 uint8_ts
static bool SD_RxDataBlock(uint8_t *buff, unsigned int btr)
{
  uint8_t token;

  uint32_t startTicks = MS_TICKS();
  //Wait for SD Response
  do
  {
    token = SPI_RxByte();
  } while((token == 0xFF) && ((MS_TICKS() - startTicks) < 100));

  if(token != 0xFE)
    return false;

  //Receive the data and write to buffer
  do
  {
    SPI_RxBytePtr(buff++);
    SPI_RxBytePtr(buff++);
  } while(btr -= 2);

  //Read CRC
  SPI_RxByte();
  SPI_RxByte();

  return true;
}

//Write Data Block
#if _READONLY == 0
static bool SD_TxDataBlock(const uint8_t *buff, uint8_t token)
{
  uint8_t resp, wc;
  uint8_t i = 0;

  if (SD_ReadyWait() != 0xFF)
    return false;

  SPI_TxByte(token);

  if (token != 0xFD)
  {
    wc = 0;

    //Transmit Block data 512 uint8_ts = 256*2
    do
    {
      SPI_TxByte(*buff++);
      SPI_TxByte(*buff++);
    } while (--wc);

    SPI_RxByte();       /* CRC 무시 */
    SPI_RxByte();

    //Data response receive
    while (i <= 64)
    {
      resp = SPI_RxByte();

      //Handle Error
      if ((resp & 0x1F) == 0x05)
        break;

      i++;
    }

    //Clear Receive Buffer
    while (SPI_RxByte() == 0);
  }

  if ((resp & 0x1F) == 0x05)
    return true;
  else
    return false;
}
#endif /* _READONLY */

//Send Command
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg)
{
  uint8_t crc, res;

  //Get SD Ready
  if (SD_ReadyWait() != 0xFF)
    return 0xFF;

  //Build command
  SPI_TxByte(cmd);      /* Command */
  SPI_TxByte((uint8_t) (arg >> 24));   /* Argument[31..24] */
  SPI_TxByte((uint8_t) (arg >> 16));   /* Argument[23..16] */
  SPI_TxByte((uint8_t) (arg >> 8));  /* Argument[15..8] */
  SPI_TxByte((uint8_t) arg);     /* Argument[7..0] */

  crc = 0;
  if (cmd == CMD0)
    crc = 0x95; /* CRC for CMD0(0) */

  if (cmd == CMD8)
    crc = 0x87; /* CRC for CMD8(0x1AA) */

  SPI_TxByte(crc);

  /* CMD12 Stop Reading, Discard 1 response uint8_t */
  if (cmd == CMD12)
    SPI_RxByte();

  //Response within 10 tries
  uint8_t n = 10;
  do
  {
    res = SPI_RxByte();
  } while ((res & 0x80) && --n);

  return res;
}

//*** Low-Level functions ***//
//Get Sector Count
bool sd_driver_ll_getSectorCount(uint32_t *sectorCount)
{
  uint8_t n, csd[16];
  uint32_t csize;
  bool ret=0;
  SELECT();

  if ((SD_SendCmd(CMD9, 0) == 0) && SD_RxDataBlock(csd, 16))
  {
    if ((csd[0] >> 6) == 1)
    {
      /* SDC ver 2.00 */
      csize = csd[9] + ((uint32_t) csd[8] << 8) + 1;
      *sectorCount = (uint32_t) csize << 10;
    }
    else
    {
      /* MMC or SDC ver 1.XX */
      n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
      csize = (csd[8] >> 6) + ((uint32_t) csd[7] << 2) + ((uint32_t) (csd[6] & 3) << 10) + 1;
      *sectorCount = (uint32_t) csize << (n - 9);
    }
    ret = true;
  }
  DESELECT();
  SPI_RxByte();
  return ret;
}
//Get Sector Size
bool sd_driver_ll_getSectorSize(uint32_t *sectorSize)
{
  *sectorSize=512;
  return true;
}
//Control Power
void sd_driver_ll_controlPower(bool stateOnOff)
{
  if(stateOnOff)
  {
    SD_PowerOn();   /* Power On */
  }
  else
  {
    SD_PowerOff();  /* Power Off */
  }
}
bool sd_driver_ll_getPowerState(void)
{
  return SD_CheckPower();
}
//Control Sync
bool sd_driver_ll_sync(void)
{
  bool ret=0;
  SELECT();
  if (SD_ReadyWait() == 0xFF)
  {
    ret= true;
  }
  DESELECT();
  SPI_RxByte();
  return ret;
}
//Get CSD
bool sd_driver_ll_getCSD(uint8_t *pBuf16uint8_ts)
{
  bool ret=0;
  SELECT();
  uint8_t *ptr = pBuf16uint8_ts;
  /* CSD information (16 uint8_ts) */
  if (SD_SendCmd(CMD9, 0) == 0 && SD_RxDataBlock(ptr, 16))
  {
    ret = true;
  }
  DESELECT();
  SPI_RxByte();
  return ret;
}
//Get CID
bool sd_driver_ll_getCID(uint8_t *pBuf16uint8_ts)
{
  uint8_t *ptr = pBuf16uint8_ts;
  bool ret=0;
  SELECT();
  /* CID information (16 uint8_ts) */
  if (SD_SendCmd(CMD10, 0) == 0 && SD_RxDataBlock(ptr, 16))
  {
    ret = true;
  }
  DESELECT();
  SPI_RxByte();
  return ret;
}
//Get OCR
bool sd_driver_ll_getOCR(uint8_t *pBuf4uint8_ts)
{
  uint8_t n, *ptr = pBuf4uint8_ts;
  bool ret=0;
  SELECT();
  /* OCR information (4 uint8_ts) */
  if (SD_SendCmd(CMD58, 0) == 0)
  {
    for (n = 0; n < 4; n++)
    {
      *ptr++ = SPI_RxByte();
    }
    ret = true;
  }
  DESELECT();
  SPI_RxByte();
  return ret;
}

/**
 * @brief SD Card Initialise
 */
DiskStatus_e sd_driver_init(void)
{
  uint8_t n, type, ocr[4];
  //Card Not inserted
  if(Stat & STATUS_NODISK)
  {
    return Stat;
  }
  //Power ON Sequence
  SD_PowerOn();
  SELECT();
  type = 0;

  //Idle State
  if (SD_SendCmd(CMD0, 0) == 1)
  {
    uint32_t startTicks = MS_TICKS();
    //Read SD Card Interface specifications
    if (SD_SendCmd(CMD8, 0x1AA) == 1)
    {
      /* SDC Ver2+ */
      for (n = 0; n < 4; n++)
      {
        ocr[n] = SPI_RxByte();
      }

      if (ocr[2] == 0x01 && ocr[3] == 0xAA)
      {
        /* Voltage Range 2.7-3.6V */
        do {
          if (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 1UL << 30) == 0)
            break; /* ACMD41 with HCS bit */
        } while ((MS_TICKS() - startTicks) < 1000);

        if (((MS_TICKS() - startTicks) < 1000) && SD_SendCmd(CMD58, 0) == 0)
        {
          /* Check CCS bit */
          for (n = 0; n < 4; n++)
          {
            ocr[n] = SPI_RxByte();
          }

          type = (ocr[0] & 0x40) ? 6 : 2;
        }
      }
    }
    else
    {
      /* SDC Ver1 or MMC */
      type = (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 0) <= 1) ? 2 : 1; /* SDC : MMC */

      do {
        if (type == 2)
        {
          if (SD_SendCmd(CMD55, 0) <= 1 && SD_SendCmd(CMD41, 0) == 0)
            break; /* ACMD41 */
        }
        else
        {
          if (SD_SendCmd(CMD1, 0) == 0)
            break; /* CMD1 */
        }
      } while ((MS_TICKS() - startTicks) < 1000);

      if (((MS_TICKS() - startTicks) > 1000) || SD_SendCmd(CMD16, 512) != 0)
      {
        //Block Length based on type
        type = 0;
      }
    }
  }

  CardType = type;
  DESELECT();
  SPI_RxByte(); /* Idle State exist */
  if (type)
  {
    /* Clear STATUS_NOINIT */
    Stat &= ~STATUS_NOINIT;
  }
  else
  {
    /* Initialization failed */
    SD_PowerOff();
  }
  return Stat;
}

/**
 * @brief SD Card Status
 */
SD_State_e sd_driver_getState(void)
{
  return Stat;
}

/**
 * @brief SD Card Size info
 */
void sd_driver_getSizeInfo(uint32_t *pBlockSize, uint32_t *pBlockNbr)
{
  sd_driver_ll_getSectorCount(pBlockNbr);
  sd_driver_ll_getSectorSize(pBlockSize);
}

/**
 * @brief Read Block
 */
bool sd_driver_readBlocks(uint8_t *pData, uint32_t readAddr, uint32_t numOfBlocks)
{
  if (Stat & STATUS_NOINIT)
    return false;
  if (!(CardType & 4))
    readAddr *= 512; //Sector size
  SELECT();

  if (numOfBlocks == 1)
  {
    //Read single Block
    if ((SD_SendCmd(CMD17, readAddr) == 0) && SD_RxDataBlock(pData, 512))
      numOfBlocks = 0;
  }
  else
  {
    //Read Multiple Blocks
    if (SD_SendCmd(CMD18, readAddr) == 0)
    {
      do {
        if (!SD_RxDataBlock(pData, 512))
          break;

        pData += 512;
      } while (--numOfBlocks);

      /* STOP_TRANSMISSION request send */
      SD_SendCmd(CMD12, 0);
    }
  }
  DESELECT();
  SPI_RxByte(); /* Idle Exit */
  return numOfBlocks ? false : true;
}

/**
 * @brief Write Block
 */
bool sd_driver_writeBlocks(uint8_t *pData, uint32_t writeAddr, uint32_t numOfBlocks)
{
  if (Stat & STATUS_NOINIT)
    return false;

  if (Stat & STATUS_PROTECT)
    return false;

  if (!(CardType & 4))
    writeAddr *= 512; //Sector size

  SELECT();

  if (numOfBlocks == 1)
  {
    //Write Single Block
    if ((SD_SendCmd(CMD24, writeAddr) == 0) && SD_TxDataBlock(pData, 0xFE))
      numOfBlocks = 0;
  }
  else
  {
    //Write Multiple Blocks
    if (CardType & 2)
    {
      SD_SendCmd(CMD55, 0);
      SD_SendCmd(CMD23, numOfBlocks); /* ACMD23 */
    }

    if (SD_SendCmd(CMD25, writeAddr) == 0)
    {
      do {
        if(!SD_TxDataBlock(pData, 0xFC))
          break;

        pData += 512;
      } while (--numOfBlocks);

      if(!SD_TxDataBlock(0, 0xFD))
      {
        numOfBlocks = 1;
      }
    }
  }

  DESELECT();
  SPI_RxByte();

  return numOfBlocks ? false : true;
}

