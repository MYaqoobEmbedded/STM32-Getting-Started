/*
 * i2c.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "i2c.h"

/**
 * @brief I2C GPIO configuration
 */
void i2c_I2C1_gpioConfig(void)
{
  //Enable Port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  /* PB8-SCL and PB9-SDA */
  //Mode: Output/Alternate, Speed: 10MHz
  GPIOB->CRH &= ~(GPIO_CRH_MODE8);
  GPIOB->CRH &= ~(GPIO_CRH_MODE9);
  GPIOB->CRH |= (GPIO_CRH_MODE8_0);
  GPIOB->CRH |= (GPIO_CRH_MODE9_0);
  //Alternate function open-drain
  GPIOB->CRH |= (GPIO_CRH_CNF8);
  GPIOB->CRH |= (GPIO_CRH_CNF9);
  //Remap I2C1
  AFIO->MAPR |= (1UL << 1);
}

/**
 * @brief I2C Peripheral configuration
 */
void i2c_I2C1_config(void)
{
  //Enable I2C peripheral clock
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  /* I2C1 registers configurations */
  //Set APB1 Clock frequency - 36MHz
  I2C1->CR2 |= (36);
  //Set Rise time, based on Mode (Regular < 100KHz, Fast mode > 100KHZ)
  I2C1->TRISE = 36 + 1;
  //I2C speed (100KHz == 0xD2)
  I2C1->CCR = 0xD2;
  //Enable I2C peripheral
  I2C1->CR1 |= I2C_CR1_PE;
}

/**
 * @brief I2C Check Address
 */
bool i2c_I2C1_isSlaveAddressExist(uint8_t addrs, uint32_t timeout)
{
  uint32_t initTicks = rcc_msGetTicks();
  //Wait for busy flag
  while(I2C1->SR2 & I2C_SR2_BUSY)
  {
    if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
  }
  //Clear POS bit
  I2C1->CR1 &= ~I2C_CR1_POS;
  //Generate start condition
  I2C1->CR1 |= I2C_CR1_START;
  //Wait for Start Condition generation
  while(!(I2C1->SR1 & I2C_SR1_SB))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Send slave address
  I2C1->DR = addrs;
  //Wait for ADDR flag (confirms Ack is received from Slave device)
  while(!(I2C1->SR1 & I2C_SR1_ADDR))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Generate Stop condition
  I2C1->CR1 |= I2C_CR1_STOP;
  //Clear ADDR flag
  __I2C_CLEAR_ADDRFLAG();
  //Wait for busy flag
  while(I2C1->SR2 & I2C_SR2_BUSY)
  {
    if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
  }
  return 1;
}

/**
 * @brief I2C Transmit
 */
bool i2c_I2C1_masterTransmit(uint8_t addrs, uint8_t *pData, uint8_t len, uint32_t timeout)
{
  uint32_t initTicks = rcc_msGetTicks();
  //Wait for busy flag
  while(I2C1->SR2 & I2C_SR2_BUSY)
  {
    if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
  }
  //Clear POS bit
  I2C1->CR1 &= ~I2C_CR1_POS;
  //Generate start condition
  I2C1->CR1 |= I2C_CR1_START;
  //Wait for Start Condition generation
  while(!(I2C1->SR1 & I2C_SR1_SB))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Send Address, with LSB=0 for Transmit
  //Send slave address
  I2C1->DR = addrs;
  //Wait for ADDR flag (confirms Ack is received from Slave device)
  while(!(I2C1->SR1 & I2C_SR1_ADDR))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Clear ADDR flag
  __I2C_CLEAR_ADDRFLAG();
  //Send Data
  uint8_t dataIdx = 0;
  int8_t dataSize = len;
  while(dataSize>0)
  {
    while(!(I2C1->SR1 & I2C_SR1_TXE))
    {
      if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
    }
    I2C1->DR = pData[dataIdx];
    dataIdx++;
    dataSize--;

    //Wait on BTF
    while(!(I2C1->SR1 & I2C_SR1_BTF))
    {
      if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
    }
  }
  //Generate Stop condition
  I2C1->CR1 |= I2C_CR1_STOP;
  return 1;
}

/**
 * @brief I2C Receive
 */
bool i2c_I2C1_masterReceive(uint8_t addrs, uint8_t *pData, uint8_t len, uint32_t timeout)
{
  uint32_t initTicks = rcc_msGetTicks();
  //Wait for busy flag
  while(I2C1->SR2 & I2C_SR2_BUSY)
  {
    if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
  }
  //Clear POS bit
  I2C1->CR1 &= ~I2C_CR1_POS;

  //Enable ACK
  I2C1->CR1 |= I2C_CR1_ACK;
  //Generate start condition
  I2C1->CR1 |= I2C_CR1_START;
  //Wait for SB Flag
  while(!(I2C1->SR1 & I2C_SR1_SB))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Wait for Start Condition generation
  while(!(I2C1->SR1 & I2C_SR1_SB))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  //Send Address, with LSB=0 for Transmit
  //Send slave address
  I2C1->DR = addrs | 0x01;
  //Wait for ADDR flag (confirms Ack is received from Slave device)
  while(!(I2C1->SR1 & I2C_SR1_ADDR))
  {
    if((rcc_msGetTicks() - initTicks) > timeout) return 0;
  }
  if(len==0)
  {
    //Clear ADDR flag
    __I2C_CLEAR_ADDRFLAG();
    //Generate Stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
    return 0;
  }
  else if(len == 1)
  {
    //Clear ACK
    I2C1->CR1 &= ~I2C_CR1_ACK;
    //Disable interrupts
    __disable_irq();
    //Clear ADDR flag
    __I2C_CLEAR_ADDRFLAG();
    //Generate Stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
    //Enable interrupts
    __enable_irq();
  }
  else if(len == 2)
  {
    //Enable POS bit
    I2C1->CR1 |= I2C_CR1_POS;
    //Disable interrupts
    __disable_irq();
    //Clear ADDR flag
    __I2C_CLEAR_ADDRFLAG();
    //Clear ACK
    I2C1->CR1 &= ~I2C_CR1_ACK;
    //Enable interrupts
    __enable_irq();
  }
  else
  {
    //Enable ACK
    I2C1->CR1 |= I2C_CR1_ACK;
    //Clear ADDR flag
    __I2C_CLEAR_ADDRFLAG();
  }

  //Receive data
  uint8_t dataIdx = 0;
  int8_t dataSize = len;
  while(dataSize > 0)
  {
    if(dataSize <= 3U)
    {
      //One-byte
      if(dataSize == 1U)
      {
        //Wait on RXNE
        while(!(I2C1->SR1 & I2C_SR1_RXNE))
        {
          if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
        }
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;

      }
      //Two-byte
      else if(dataSize == 2U)
      {
        //Wait on BTF
        while(!(I2C1->SR1 & I2C_SR1_BTF))
        {
          if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
        }
        //Disable interrupts
        __disable_irq();
        //Generate Stop
        I2C1->CR1 |= I2C_CR1_STOP;
        //Read data from DR
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
        //Enable interrupts
        __enable_irq();
        //Read data from DR
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
      }
      //Last 3 bytes
      else
      {
        //Wait on BTF
        while(!(I2C1->SR1 & I2C_SR1_BTF))
        {
          if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
        }
        //Clear ACK
        I2C1->CR1 &= ~I2C_CR1_ACK;
        //Disable interrupts
        __disable_irq();
        //Read data from DR
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
        //Wait on BTF
        while(!(I2C1->SR1 & I2C_SR1_BTF))
        {
          if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
        }
        //Generate Stop
        I2C1->CR1 |= I2C_CR1_STOP;
        //Read data from DR
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
        //Enable interrupts
        __enable_irq();
        //Read data from DR
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
      }
    }
    else
    {
      //Wait on RXNE
      while(!(I2C1->SR1 & I2C_SR1_RXNE))
      {
        if((rcc_msGetTicks() - initTicks)>= timeout) return 0;
      }
      pData[dataIdx] = (uint8_t)I2C1->DR;
      dataIdx++;
      dataSize--;
      if(I2C1->SR1 & I2C_SR1_BTF)
      {
        pData[dataIdx] = (uint8_t)I2C1->DR;
        dataIdx++;
        dataSize--;
      }
    }
  }
  return 1;
}

