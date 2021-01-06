/*
 * spi.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "spi.h"

/**
 * @brief SPI GPIO pins setup
 */
void spi_gpioConfig(void)
{
  //Enable Port A clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  /* PA5-SCK and PA7-MOSI */
  //Mode: Output, Speed: 10MHz
  GPIOA->CRL &= ~(GPIO_CRL_MODE5);
  GPIOA->CRL &= ~(GPIO_CRL_MODE7);
  GPIOA->CRL |= (GPIO_CRL_MODE5_0);
  GPIOA->CRL |= (GPIO_CRL_MODE7_0);
  //Alternate function push-pull
  GPIOA->CRL &= ~(GPIO_CRL_CNF5);
  GPIOA->CRL &= ~(GPIO_CRL_CNF7);
  GPIOA->CRL |= (GPIO_CRL_CNF5_1);
  GPIOA->CRL |= (GPIO_CRL_CNF7_1);

  /* PA5-MISO */
  //Mode input
  GPIOA->CRL &= ~(GPIO_CRL_MODE6);
  //Floating input
  GPIOA->CRL &= ~(GPIO_CRL_CNF6);
  GPIOA->CRL |= (GPIO_CRL_CNF6_0);

  //Remap 0: PA5, PA6, PA7
  AFIO->MAPR &= ~(1UL << 0);
}

/**
 * @brief SPI peripheral configuration
 */
void spi_config(void)
{
  //Enable SPI Clock
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  //Set clock phase to edge 2
  SPI1->CR1 |= SPI_CR1_CPHA;
  //Clock polarity, HIGH when idle
  SPI1->CR1 |= SPI_CR1_CPOL;
  //Mode: Master
  SPI1->CR1 |= SPI_CR1_MSTR;
  //Baud rate to (72MHz / 128 = 562.5Khz)
  SPI1->CR1 &= ~SPI_CR1_BR;
  SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_2;
  //MSB first
  SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);
  //Full duplex (Transmit/Receive)
  SPI1->CR1 &= ~(SPI_CR1_RXONLY);
  //Data format 8-bit
  SPI1->CR1 &= ~(SPI_CR1_DFF);
  //Software Slave select
  SPI1->CR1 |= SPI_CR1_SSI;
  SPI1->CR1 |= SPI_CR1_SSM;
  //SPI Enable
  SPI1->CR1 |= SPI_CR1_SPE;
  //Clear initial flags
  __IO uint32_t tempRd = SPI1->SR;
}

/**
 * @brief SPI Transmit
 */
bool spi_transmit(uint8_t *data, uint8_t len, uint32_t timeout)
{
  //Enable SPI if not
  SPI1->CR1 |= SPI_CR1_SPE;
  //Write to DR as TXE flag is HIGH (Tx buffer Empty)
  uint8_t dataIdx = 0;
  uint32_t startTick = rcc_msGetTicks();
  while(dataIdx<len)
  {
    if(SPI1->SR & SPI_SR_TXE) //Tx buffer empty
    {
      SPI1->DR = data[dataIdx];
      dataIdx++;
    }
    else //Manage timeout
    {
      if((rcc_msGetTicks() - startTick)>= timeout) return 0;
    }
  }
  //Wait for busy flag
  while(SPI1->SR & SPI_SR_BSY)
  {
    if((rcc_msGetTicks() - startTick)>= timeout) return 0;
  }
  //Clear over-run
  __IO uint32_t tempVar = SPI1->DR;
  tempVar = SPI1->SR;
  return 1;
}

/**
 * @brief SPI Receive
 */
bool spi_receive(uint8_t *data, uint8_t len, uint32_t timeout)
{
  //Enable SPI if not
  SPI1->CR1 |= SPI_CR1_SPE;
  //Read to DR as RXNE flag is HIGH (Rx buffer Not Empty)
  uint8_t dataIdx = 0;
  uint32_t startTick = rcc_msGetTicks();
  uint8_t dummyTx = 1;

  while(dataIdx<len)
  {
    //Transmit dummy and lock
    if((SPI1->SR & SPI_SR_TXE) && dummyTx)
    {
      SPI1->DR = 0xFF;
      dummyTx = 0;
    }

    if(SPI1->SR & SPI_SR_RXNE) //Rx buffer NOT empty
    {
      data[dataIdx] = SPI1->DR;
      dataIdx++;
      dummyTx = 1; //Next is Tx
    }
    else //Manage timeout
    {
      if((rcc_msGetTicks() - startTick)>= timeout) return 0;
    }
  }
  //Wait for busy flag
  while(SPI1->SR & SPI_SR_BSY)
  {
    if((rcc_msGetTicks() - startTick)>= timeout) return 0;
  }
  //Clear over-run
  __IO uint32_t tempVar = SPI1->DR;
  tempVar = SPI1->SR;
  return 1;
}

/**
 * @brief SPI Receive
 */
bool spi_transmitReceive(uint8_t *dataTx, uint8_t *dataRx, uint8_t len, uint32_t timeout)
{
  //Enable SPI if not
  SPI1->CR1 |= SPI_CR1_SPE;
  //Read to DR as RXNE flag is HIGH (Rx buffer Not Empty)
  uint8_t dataIdx = 0;
  uint32_t startTick = rcc_msGetTicks();
  uint8_t dummyTx = 1;

  while(dataIdx<len)
  {
    //Transmit dummy and lock
    if((SPI1->SR & SPI_SR_TXE) && dummyTx)
    {
      SPI1->DR = dataTx[dataIdx];
      dummyTx = 0;
    }

    if(SPI1->SR & SPI_SR_RXNE) //Rx buffer NOT empty
    {
      dataRx[dataIdx] = SPI1->DR;
      dataIdx++;
      dummyTx = 1; //Next is Tx
    }
    else //Manage timeout
    {
      if((rcc_msGetTicks() - startTick)>= timeout) return 0;
    }
  }
  //Wait for busy flag
  while(SPI1->SR & SPI_SR_BSY)
  {
    if((rcc_msGetTicks() - startTick)>= timeout) return 0;
  }
  //Clear over-run
  __IO uint32_t tempVar = SPI1->DR;
  tempVar = SPI1->SR;
  return 1;
}

/**
 * @brief RFID CS Pin
 */
void spi_cs_rfid_config(void)
{
  //Enable port A clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  //PA4 output mode, low-speed
  GPIOA->CRL &= ~GPIO_CRL_CNF4;
  //Push-pull
  GPIOA->CRL &= ~GPIO_CRL_MODE4;
  GPIOA->CRL |= GPIO_CRL_MODE4_1;
  //Default to HIGH
  GPIOA->ODR |= (1UL << 4);
}

/**
 * @brief RFID CS set/reset
 */
void spi_cs_rfid_write(bool state)
{
  if(state)
  {
    GPIOA->ODR |= (1UL << 4);
  }
  else
  {
    GPIOA->ODR &= ~(1UL << 4);
  }
}

/**
 * @brief SD Card SPI-CS
 */
void spi_cs_sd_config(void)
{
  //Enable port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  //PB1 output mode, low-speed
  GPIOB->CRL &= ~GPIO_CRL_CNF1;
  //Push-pull
  GPIOB->CRL &= ~GPIO_CRL_MODE1;
  GPIOB->CRL |= GPIO_CRL_MODE1_1;
  //Default to HIGH
  spi_cs_sd_write(1);
}

/**
 * @brief SD Card CS pin on/off
 */
void spi_cs_sd_write(bool state)
{
  if(state)
  {
    GPIOB->ODR |= (1UL << 1);
  }
  else
  {
    GPIOB->ODR &= ~(1UL << 1);
  }
}
