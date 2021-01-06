/*
 * rc522.c
 *
 *  Created on: 17 Jul 2020
 *      Author: Dell
 */

#include "rc522.h"
#include <string.h>
#include <stdio.h>
#include "spi.h"
#include "gpio.h"

/**
 * @brief initialize function
 */
void rc522_init(void)
{
  rc522_reset();

  rc522_regWrite8(MFRC522_REG_T_MODE, 0x80);
  rc522_regWrite8(MFRC522_REG_T_PRESCALER, 0xA9);
  rc522_regWrite8(MFRC522_REG_T_RELOAD_L, 0xE8);
  rc522_regWrite8(MFRC522_REG_T_RELOAD_H, 0x03);

  /* 48dB gain */
//  rc522_regWrite8(MFRC522_REG_RF_CFG, 0x70);

  rc522_regWrite8(MFRC522_REG_TX_AUTO, 0x40);
  rc522_regWrite8(MFRC522_REG_MODE, 0x3D);

  rc522_antennaON();   //Open the antenna
}

/**
 * @brief read register
 */
uint8_t rc522_regRead8(uint8_t reg)
{
  spi_CS1_enable();
  reg = ((reg << 1) & 0x7E) | 0x80;
  HAL_SPI_Transmit(&hspi1, &reg, 1, 100);
  uint8_t dataRd=0;
  HAL_SPI_Receive(&hspi1, &dataRd, 1, 100);
  spi_CS1_disable();
  return dataRd;
}

/**
 * @brief write register
 */
void rc522_regWrite8(uint8_t reg, uint8_t data8)
{
  spi_CS1_enable();
  uint8_t txData[2] = {0x7E&(reg << 1), data8};
  HAL_SPI_Transmit(&hspi1, txData, 2, 100);
  spi_CS1_disable();
}

/**
 * @brief set bit
 */
void rc522_setBit(uint8_t reg, uint8_t mask)
{
  rc522_regWrite8(reg, rc522_regRead8(reg)|mask);
}

/**
 * @brief clear bit
 */
void rc522_clearBit(uint8_t reg, uint8_t mask)
{
  rc522_regWrite8(reg, rc522_regRead8(reg)&(~mask));
}

/**
 * @brief reset function
 */
void rc522_reset(void)
{
  rc522_regWrite8(0x01, 0x0F);
}

/**
 * @brief Antenna ON
 */
void rc522_antennaON(void)
{
  uint8_t temp;

  temp = rc522_regRead8(MFRC522_REG_TX_CONTROL);
  if (!(temp & 0x03)) {
    rc522_setBit(MFRC522_REG_TX_CONTROL, 0x03);
  }
}

/**
 * @brief Check card
 */
bool rc522_checkCard(uint8_t *id)
{
  bool status=false;
  //Find cards, return card type
    status = rc522_request(PICC_REQIDL, id);
    if (status == true) {
      //Card detected
      //Anti-collision, return card serial number 4 bytes
      status = rc522_antiColl(id);
    }
    rc522_halt();      //Command card into hibernation

    return status;
}

/**
 * @brief Request function
 */
bool rc522_request(uint8_t reqMode, uint8_t *tagType)
{
  bool status=false;
  uint16_t backBits;
  rc522_regWrite8(MFRC522_REG_BIT_FRAMING, 0x07);
  tagType[0] = reqMode;
  status = rc522_toCard(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
  if ((status != true) || (backBits != 0x10)) {
    status = false;
  }
  return status;
}

/**
 * @brief to Card
 */
bool rc522_toCard(
    uint8_t command,
    uint8_t* sendData,
    uint8_t sendLen,
    uint8_t* backData,
    uint16_t* backLen)
{
  bool status = false;
  uint8_t irqEn = 0x00;
  uint8_t waitIRq = 0x00;
  uint8_t lastBits;
  uint8_t n;
  uint16_t i;

  switch (command) {
    case PCD_AUTHENT: {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE: {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
  }

  rc522_regWrite8(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
  rc522_clearBit(MFRC522_REG_COMM_IRQ, 0x80);
  rc522_setBit(MFRC522_REG_FIFO_LEVEL, 0x80);

  rc522_regWrite8(MFRC522_REG_COMMAND, PCD_IDLE);

  //Writing data to the FIFO
  for (i = 0; i < sendLen; i++) {
    rc522_regWrite8(MFRC522_REG_FIFO_DATA, sendData[i]);
  }

  //Execute the command
  rc522_regWrite8(MFRC522_REG_COMMAND, command);
  if (command == PCD_TRANSCEIVE) {
    rc522_setBit(MFRC522_REG_BIT_FRAMING, 0x80);   //StartSend=1,transmission of data starts
  }

  //Waiting to receive data to complete
  i = 100;  //i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
  do {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = rc522_regRead8(MFRC522_REG_COMM_IRQ);
    i--;
  } while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  rc522_clearBit(MFRC522_REG_BIT_FRAMING, 0x80);     //StartSend=0

  if (i != 0)  {
    if (!(rc522_regRead8(MFRC522_REG_ERROR) & 0x1B)) {
      status = true;
      if (n & irqEn & 0x01) {
        status = false;
      }

      if (command == PCD_TRANSCEIVE) {
        n = rc522_regRead8(MFRC522_REG_FIFO_LEVEL);
        uint8_t l = n;
        lastBits = rc522_regRead8(MFRC522_REG_CONTROL) & 0x07;
        if (lastBits) {
          *backLen = (n - 1) * 8 + lastBits;
        } else {
          *backLen = n * 8;
        }

        if (n == 0) {
          n = 1;
        }
        if (n > MFRC522_MAX_LEN) {
          n = MFRC522_MAX_LEN;
        }

        //Reading the received data in FIFO
        for (i = 0; i < n; i++) {
          uint8_t d = rc522_regRead8(MFRC522_REG_FIFO_DATA);
          if (l == 4)
            printf("%02x ", d);
          backData[i] = d;
        }
        if (l==4)
          printf("\r\n");
        return status;
      }
    } else {
      printf("error\r\n");
      status = false;
    }
  }

  return status;
}

bool rc522_antiColl(uint8_t* serNum)
{
  bool status;
  uint8_t i;
  uint8_t serNumCheck = 0;
  uint16_t unLen;
  //for (i = 0; i < 4; i++)
//    printf("Anticoll In %d: 0x%02x\r\n", i, serNum[i]);


  rc522_regWrite8(MFRC522_REG_BIT_FRAMING, 0x00);    //TxLastBists = BitFramingReg[2..0]

  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  status = rc522_toCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

  //for (i = 0; i < 4; i++)
//      printf("Anticoll ToCard %d: 0x%02x\r\n", i, serNum[i]);

  if (status == true) {
    //Check card serial number
    for (i = 0; i < 4; i++) {
      serNumCheck ^= serNum[i];
    }
    if (serNumCheck != serNum[i]) {
      status = false;
    }
  }
  return status;
}

void rc522_halt(void)
{
  uint16_t unLen;
  uint8_t buff[4];

  buff[0] = PICC_HALT;
  buff[1] = 0;
  rc522_calculateCRC(buff, 2, &buff[2]);

  rc522_toCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

void rc522_calculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData)
{
  uint8_t i, n;

  rc522_clearBit(MFRC522_REG_DIV_IRQ, 0x04);     //CRCIrq = 0
  rc522_setBit(MFRC522_REG_FIFO_LEVEL, 0x80);      //Clear the FIFO pointer
  //Write_MFRC522(CommandReg, PCD_IDLE);

  //Writing data to the FIFO
  for (i = 0; i < len; i++) {
    rc522_regWrite8(MFRC522_REG_FIFO_DATA, *(pIndata+i));
  }
  rc522_regWrite8(MFRC522_REG_COMMAND, PCD_CALCCRC);

  //Wait CRC calculation is complete
  i = 0xFF;
  do {
    n = rc522_regRead8(MFRC522_REG_DIV_IRQ);
    i--;
  } while ((i!=0) && !(n&0x04));      //CRCIrq = 1

  //Read CRC calculation result
  pOutData[0] = rc522_regRead8(MFRC522_REG_CRC_RESULT_L);
  pOutData[1] = rc522_regRead8(MFRC522_REG_CRC_RESULT_M);
}

/**
 * @brief compare IDs
 */
bool rc522_compareIds(uint8_t *idCurrent, uint8_t *idReference)
{
  uint8_t i;
  for(i=0; i<5;i++)
  {
    if(idCurrent[i] != idReference[i])
    {
      return false;
    }
  }
  return true;
}
