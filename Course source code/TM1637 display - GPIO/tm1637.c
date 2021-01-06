/*
 * tm1637.c
 *
 *  Created on: 26 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#include "tm1637.h"
#include "dwt_delay.h"
#include "rcc.h"

//#define __USE_HAL

//GPIO pins
static GPIO_TypeDef *cdPort;
static uint16_t clockPin;
static uint16_t dataPin;
static uint8_t _brightness=0;
static bool _colomn=false;

#ifdef __USE_HAL
#define MICRO_DELAY(x) (HAL_Delay(1))
#else
#define MICRO_DELAY(x) (rcc_msDelay(1))
#endif

//List of digits
static uint8_t digitsToSegment[10] =
{
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F,
};

typedef enum DIO_Mode_TAG
{
  DIO_Mode_Output=0,
  DIO_Mode_Input,
}DIO_Mode_e;

//**** Static Functions ****//
// PROTOTYPES
void tm1637_changeDioMode(DIO_Mode_e mode);
void tm1637_setCLK(bool state);
void tm1637_setDIO(bool state);
bool tm1637_getDIO(void);
void tm1637_TWI_start(void);
void tm1637_TWI_ack(void);
void tm1637_TWI_stop(void);
void tm1637_TWI_writeByte(uint8_t data8);


// DEFINITIONS
//Change DIO Mode
void tm1637_changeDioMode(DIO_Mode_e mode)
{
#ifdef __USE_HAL
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = dataPin;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  if(mode == DIO_Mode_Output)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  }
  else if(mode == DIO_Mode_Input)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  }
  HAL_GPIO_Init(cdPort, &GPIO_InitStruct);
#else
  return;
#endif

}
//Clock pin state
void tm1637_setCLK(bool state)
{
#ifdef __USE_HAL
  HAL_GPIO_WritePin(cdPort, clockPin, (GPIO_PinState)state);
#else
  if(state)
  {
    cdPort->ODR |= clockPin;
  }
  else
  {
    cdPort->ODR &= ~clockPin;
  }
#endif
}
//DIO pin state
void tm1637_setDIO(bool state)
{
#ifdef __USE_HAL
  HAL_GPIO_WritePin(cdPort, dataPin, (GPIO_PinState)state);
#else
  if(state)
  {
    cdPort->ODR |= dataPin;
  }
  else
  {
    cdPort->ODR &= ~dataPin;
  }
#endif
}
//Read DIO in input mode
bool tm1637_getDIO(void)
{
#ifdef __USE_HAL
  return (bool)HAL_GPIO_ReadPin(cdPort, dataPin);
#else
  return (bool)(cdPort->IDR & dataPin);
#endif
}
//Start sequence
void tm1637_TWI_start(void)
{
  tm1637_setCLK(1);
  tm1637_setDIO(1);
  MICRO_DELAY(2);
  tm1637_setDIO(0);
}
//Wait for Ack
void tm1637_TWI_ack(void)
{
  tm1637_setCLK(0);
  MICRO_DELAY(5);
  tm1637_setCLK(1);
  MICRO_DELAY(2);
  tm1637_setCLK(0);
}
//Stop sequence
void tm1637_TWI_stop(void)
{
  tm1637_setCLK(0);
  MICRO_DELAY(2);
  tm1637_setDIO(0);
  MICRO_DELAY(2);
  tm1637_setCLK(1);
  MICRO_DELAY(2);
  tm1637_setDIO(1);
}
//Write byte
void tm1637_TWI_writeByte(uint8_t data8)
{
  uint8_t i;
  for(i=0; i<8;i++)
  {
    tm1637_setCLK(0);
    if(data8 & 0x01)
    {
      tm1637_setDIO(1);
    }
    else
    {
      tm1637_setDIO(0);
    }
    MICRO_DELAY(3);
    data8 = data8 >> 1;
    tm1637_setCLK(1);
    MICRO_DELAY(3);
  }
}

//---------------------------------------------------//

/**
 * @brief initialisation
 */
void tm1637_init(GPIO_TypeDef *port, uint16_t clkPin, uint16_t dioPin)
{
  cdPort = port;
  clockPin = clkPin;
  dataPin = dioPin;
  tm1637_setBrightness(4);
}

/**
 * @brief write display data
 */
void tm1637_writeDigits(uint8_t digitsData[4], bool colomn)
{
  uint8_t i;
  digitsData[0] = digitsToSegment[digitsData[0]];
  digitsData[1] = digitsToSegment[digitsData[1]] | (colomn?0x80:0x00);
  digitsData[2] = digitsToSegment[digitsData[2]];
  digitsData[3] = digitsToSegment[digitsData[3]];
  //RAM Address comman - auto increment
  tm1637_TWI_start();
  tm1637_TWI_writeByte(0x40);
  tm1637_TWI_ack();
  tm1637_TWI_stop();
  //Send Address and Data
  tm1637_TWI_start();
  tm1637_TWI_writeByte(0xC0);
  tm1637_TWI_ack();
  for(i=0;i<4;i++)
  {
    tm1637_TWI_writeByte(digitsData[i]);
    tm1637_TWI_ack();
  }
  tm1637_TWI_stop();
  tm1637_setBrightness(_brightness);
}

/**
 * @brief Set Brightness
 */
void tm1637_setBrightness(uint8_t value_0_7)
{
  //Set Brightness
  _brightness = value_0_7;
  tm1637_TWI_start();
  tm1637_TWI_writeByte(0x88 + (_brightness&0x7));
  tm1637_TWI_ack();
  tm1637_TWI_stop();
}

/**
 * @brief Counter display
 */
void tm1637_setCounter(uint16_t counter)
{
  //split counter into 1000s, 100s, 10s and 1s
  //1000s
  uint8_t DigitsArray[4];
  DigitsArray[0] = counter/1000;
  counter = counter - DigitsArray[0]*1000;
  //100s
  DigitsArray[1] = counter/100;
  counter = counter - DigitsArray[1]*100;
  //10s
  DigitsArray[2] = counter/10;
  counter = counter - DigitsArray[2]*10;
  //1s
  DigitsArray[3] = counter;
  tm1637_writeDigits(DigitsArray, 0);
}

/**
 * @brief Set Clock
 */
void tm1637_setClock(uint8_t hrs, uint8_t mins)
{
  uint8_t DigitsArray[4];
  //Get Hrs digits
  DigitsArray[0] = hrs/10;
  hrs = hrs - DigitsArray[0]*10;
  DigitsArray[1] = hrs;
  //Get Min digits
  DigitsArray[2] = mins/10;
  mins = mins - DigitsArray[2]*10;
  DigitsArray[3] = mins;
  _colomn ^=1;
  tm1637_writeDigits(DigitsArray, _colomn);
}
