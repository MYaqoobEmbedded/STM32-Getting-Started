/*
 * adc.c
 *
 *  Created on: 7 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "adc.h"

/**
 * @brief ADC Calibrate
 */
void adc_calibrate(void)
{
  //Start calibration
  ADC1->CR2 |= (1UL << 2);
  //Wait for end of calibration
  while((ADC1->CR2) & (1UL << 2));
}

/**
 * @brief ADC Start
 */
void adc_start(void)
{
  ADC1->SR = 0;
  ADC1->CR2 |= (1UL << 22);
  ADC1->CR2 |= (1UL << 0); //Page 218
}

/**
 * @brief ADC Stop
 */
void adc_stop(void)
{
  ADC1->SR = 0;
  ADC1->CR2 &= ~(1UL << 22);
}

/**
 * @brief ADC channels GPIO configuration
 */
void adc_GPIO_config(void)
{
  //Enable port A clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  //ADC pins; PA1
  //Mode: Analog
  GPIOA->CRL &= ~(GPIO_CRL_MODE1);
  GPIOA->CRL &= ~(GPIO_CRL_CNF1);
  //ADC pins; PA2
  //Mode: Analog
  GPIOA->CRL &= ~(GPIO_CRL_MODE2);
  GPIOA->CRL &= ~(GPIO_CRL_CNF2);
  //ADC pins; PA3
  //Mode: Analog
  GPIOA->CRL &= ~(GPIO_CRL_MODE3);
  GPIOA->CRL &= ~(GPIO_CRL_CNF3);
}

/**
 * @brief ADC single channel configuration
 */
void adc_singleConfig(ADC_SingleSelect_e channel)
{
  //Enable ADC clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  //Right Align DR
  ADC1->CR2 &= ~(1UL << 11);
  //Single conversion
  ADC1->CR2 &= ~(1UL << 1);
  //Software trigger (SWSTART)
  ADC1->CR2 |= (7UL << 17);
  //Sample time to 28 Cycles = (12 + 28)/12MHz = 3.3us
  ADC1->SMPR2 &= ~(7UL << 3);
  ADC1->SMPR2 |= (3UL << 3);
  //Rank 1 --> PA1, PA2 or PA3, Channel1
  ADC1->SQR3 = channel;
  //Power up ADC
  ADC1->CR2 |= (1UL << 0);
  //Wait for ADC clock to stabilise (couple micro seconds)
  for(uint16_t i=0; i<36;i++); //72 cycles = 1uSec, for loop 4 cycles each, 72*2/4 = 36
}

/**
 * @brief Poll for end of conversion
 */
bool adc_pollForEndOfConversion(uint32_t msTimeout)
{
  uint32_t initTicks = rcc_msGetTicks();
  while(((ADC1->SR)&0x02)==0)
  {
    if((rcc_msGetTicks() - initTicks) > msTimeout)
    {
      return 0;
    }
  }
  return 1;
}

/**
 * @brief Read value
 */
uint16_t adc_readValue(void)
{
  return (ADC1->DR)&0xFFF; //12-bits resolution Right Aligned
}

/**
 * @brief ADC Enable Interrupt
 */
void adc_enableIT(void)
{
  //Enable EOCIE
  ADC1->CR1 |= (1UL << 5);
  //ADC1 NVIC interrupt
  NVIC_EnableIRQ(ADC1_IRQn);
}

void adc_disableIT(void)
{
  //Enable EOCIE
  ADC1->CR1 &= ~(1UL << 5);
  //ADC1 NVIC interrupt
  NVIC_DisableIRQ(ADC1_IRQn);
}

/**
 * @brief ADC Multi-channel
 */
void adc_multiChannelConfig(void)
{
  //Enable ADC clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  //Right Align DR
  ADC1->CR2 &= ~(1UL << 11);
  //Sample time to 28 Cycles = (12 + 28)/12MHz = 3.3us
  ADC1->SMPR2 &= ~(7UL << 3); //Rank1
  ADC1->SMPR2 |= (3UL << 3);
  ADC1->SMPR2 &= ~(7UL << 6); //Rank2
  ADC1->SMPR2 |= (3UL << 6);
  ADC1->SMPR2 &= ~(7UL << 9); //Rank3
  ADC1->SMPR2 |= (3UL << 9);
//  //Software trigger (SWSTART)
//  ADC1->CR2 |= (7UL << 17);
  //External trigger - rising edge
  ADC1->CR2 |= (1UL << 20);
  //TIM3 TRGO
  ADC1->CR2 &= ~(7UL << 17);
  ADC1->CR2 |= (0x4UL << 17);

  //Sequencer
  ADC1->SQR3 = (1UL << 0*5); //Rank 1 --> PA1, Channel1
  ADC1->SQR3 |= (2UL << 1*5); //Rank 2 --> PA2, Channel2
  ADC1->SQR3 |= (3UL << 2*5); //Rank 3 --> PA3, Channel3
  //No. of conversion = 3
  ADC1->SQR1 &= ~(0xFUL << 20);
  ADC1->SQR1 |= (2UL << 20);//2: 3 Conversions

  //Enable Scan mode
  ADC1->CR1 |= (1UL << 8);
  //Enable DMA
  ADC1->CR2 |= (1UL << 8);
  //Enable ADC Single conversion cycle
  ADC1->CR2 &= ~(1UL << 1);
  //Power up ADC
  ADC1->CR2 |= (1UL << 0);
  //Wait for ADC clock to stabilise (couple micro seconds)
  for(uint16_t i=0; i<36;i++); //72 cycles = 1uSec, for loop 4 cycles each, 72*2/4 = 36
}

void adc_multiChannel_DMA_config(uint16_t *pAdcBuffer)
{
  //Enable DMA1 Clock
  RCC->AHBENR |= (1UL << 0);

  //DMA1_Channel1 clear flags
  DMA1->IFCR = 0xF;

  //Peripheral address
  DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));
  //Memory address
  DMA1_Channel1->CMAR = (uint32_t)pAdcBuffer;
  //no. of transfers
  DMA1_Channel1->CNDTR = 3;
  //Circular
  DMA1_Channel1->CCR |= (1UL << 5);
  //Enable memory increment
  DMA1_Channel1->CCR |= (1UL << 7);
  //Disable Peripheral increment
  DMA1_Channel1->CCR &= ~(1UL << 6);
  //Set peripheral data size to 16
  DMA1_Channel1->CCR &= ~(3UL << 8);
  DMA1_Channel1->CCR |= (1UL << 8);
  //Set Memory data size to 16
  DMA1_Channel1->CCR &= ~(3UL << 10);
  DMA1_Channel1->CCR |= (1UL << 10);
  //Direction = Periph to Mem
  DMA1_Channel1->CCR &= ~(1UL << 4);
  //Enable DMA Channel
  DMA1_Channel1->CCR |= (1UL << 0);

  /** Enable Transfer Complete Interrupt **/
  DMA1_Channel1->CCR |= DMA_CCR_TCIE;
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
 * @brief ADC analog watchdog configure
 */
void adc_AWD_Config(uint8_t channel, uint16_t lowThreshold, uint16_t highThreshold)
{
  //Set AWD Low Threshold
  ADC1->LTR = lowThreshold;
  //Set AWD High Threshold
  ADC1->HTR = highThreshold;
  //Select AWD channel
  ADC1->CR1 &= ~(0x0000001F);
  ADC1->CR1 |= (channel << 0);
  //Enable AWDIE Interrupt
  ADC1->CR1 |= ADC_CR1_AWDIE;
  //AWD on Single Channel
  ADC1->CR1 |= ADC_CR1_AWDSGL;
  //Disable Injected
  ADC1->CR1 &= ~(ADC_CR1_JAWDEN);
  //Enable Regular Channel AWD
  ADC1->CR1 |= ADC_CR1_AWDEN;
  NVIC_EnableIRQ(ADC1_IRQn);
}

/**
 * @brief ADC Injected configure
 */
void adc_INJ_Configure(uint8_t channel)
{
  //Disable Auto Inject
  ADC1->CR1 &= ~(1UL << 10);
  //Enable JEOC interrupt
  ADC1->CR1 |= (1UL << 7);
  //Software trigger for Injected
  ADC1->CR2 |= (0xFUL << 12);
  //Injected no. of conversion =
  ADC1->JSQR &= ~(3UL << 20);
  //Set channel injected
  ADC1->JSQR &= ~(0x1FUL << 15);
  ADC1->JSQR |= (channel << 15);
  //Enable JDisc mode
  ADC1->CR1 |= (1UL << 12);
  //Sample time of CH3
  ADC1->SMPR2 &= ~(7UL << 9);
  ADC1->SMPR2 |= (3UL << 9);
  //Software start
  ADC1->CR2 |= (15UL << 12);
  //Enable ADC (ADC_ON)
  ADC1->CR2 |= (1UL << 0);
}

/**
 * @brief ADC Injected Start
 */
void adc_INJ_Start(void)
{
  ADC1->CR2 |= (1UL << 21);
}

/**
 * @brief ADC poll for end of conversion
 */
bool adc_INJ_pollForConversion(uint32_t timeout)
{
  uint32_t initTicks = rcc_msGetTicks();
  while(((ADC1->SR)&(1UL << 2))==0)
  {
    if((rcc_msGetTicks() - initTicks) > timeout)
    {
      return 0;
    }
  }
  return 1;
}

