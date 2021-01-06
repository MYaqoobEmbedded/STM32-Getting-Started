/*
 * tim.c
 *
 *  Created on: 7 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#include "tim.h"

/**
 * @brief TIM3 ADC Trigger timer configuration
 */
void tim_TIM3_config(void)
{
  //TIM3 will be used
  //Enable TIM3 clock
  RCC->APB1ENR |= (1UL << 1);
  //Count up
  TIM3->CR1 &= ~(1UL << 4);
  //Disable one pulse
  TIM3->CR1 &= ~(1UL << 3);
  //Update request source and update disable to UEV
  TIM3->CR1 &= ~(TIM_CR1_URS | TIM_CR1_UDIS);
  //TRGO mode
  TIM3->CR2 &= ~(7UL << 4);
  TIM3->CR2 |= (2UL << 4);
  //Timer prescalar and period to achieve 100Hz sampling (72MHz perph clock)
  TIM3->PSC = 7200-1; //10Khz
  TIM3->ARR = 1000-1;  //100Hz
  //Reset Counter at Timer Event
  TIM3->EGR = 1;
  //Start Timer
  TIM3->CR1 |= (1UL<<0);
}

/**
 * @brief Delay timer - TIM2
 */
void tim_TIM2_delayConfig(void)
{
  //Enable TIM2 clock
  RCC->APB1ENR |= (1UL << 0);
  //Count up
  TIM2->CR1 &= ~(1UL << 4);
  //Enable one pulse
  TIM2->CR1 |= (1UL << 3);
  //Only counter overflow generates event
  TIM2->CR1 |= TIM_CR1_URS;
  //Update Event disabled
  TIM2->CR1 &= ~(TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM2->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM2->PSC = 0;
  //ARR
  TIM2->ARR = 7200-1;   //10Khz
}

/**
 * @brief milli second delay with TIM2
 */
void tim_TIM2_msDelay(uint32_t ms)
{
  //Prescaler
  TIM2->PSC = (ms*10)-1;
  //Start TIM2
  TIM2->CR1 |= 0x01;
  //Wait until TIM count overflows
  while(!(TIM2->SR&0x01));
  //Clear TIM flag
  TIM2->SR &= ~(1UL);
}

/**
 * @brief Periodic TIM interrupt - TIM2
 */
void tim_TIM2_periodicConfig(uint32_t msPeriod)
{
  //Enable TIM2 clock
  RCC->APB1ENR |= (1UL << 0);
  //Count up
  TIM2->CR1 &= ~(1UL << 4);
  //Disable one pulse
  TIM2->CR1 &= ~(1UL << 3);
  //Only counter overflow generates event
  TIM2->CR1 |= TIM_CR1_URS;
  //Update Event enabled
  TIM2->CR1 &= ~(TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM2->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM2->PSC = 7200-1; //10KHz
  //ARR
  TIM2->ARR = (10*msPeriod)-1;   //1ms (1KHz)
  //Clear Update Event Interrupt
  TIM2->SR &= ~(1UL<<0);

  //Enable Update interrupt
  TIM2->DIER |= 0x01;
  //Enable NVIC Interrupt
  NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief TIM1_OC GPIO Config (LED1-PB13)
 */
void tim_TIM1_GPIO_config(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  AFIO->MAPR &= ~(7UL << 24);
  AFIO->MAPR |= (4UL << 24);
  //TIM_OC pin --> PB13
  //Enable port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  //Set pin mode to Output
  GPIOB->CRH &= ~(3UL << 20);
  GPIOB->CRH |= (2UL << 20);
  //Alternate push-pull
  GPIOB->CRH &= ~(3UL << 22);
  GPIOB->CRH |= (2UL << 22);
}

/**
 * @brief TIM1_OC Config
 */
void tim_TIM1_OC_config(void)
{
  //Enable TIM1 clock
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  //Count up
  TIM1->CR1 &= ~(1UL << 4);
  //Disable One pulse
  TIM1->CR1 &= ~(TIM_CR1_OPM);
  //Update request source and update disable to UEV
  TIM1->CR1 &= ~(TIM_CR1_URS | TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM1->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM1->PSC = 7200-1; //10KHz
  //ARR
  TIM1->ARR = (100*10)-1;   //500ms (2Hz)
  //Clear Update Event Interrupt
  TIM1->SR &= ~(1UL<<0);

  //Disable OC
  TIM1->CCER=0;
  //Set CH1N as output
  TIM1->CCMR1 &= ~(3UL << 0);
  //Toggle output on event
  TIM1->CCMR1 &= ~(7UL << 4);
  TIM1->CCMR1 |= (3UL << 4);
  //Set output polarity to HIGH
  TIM1->CCER &= ~(1UL << 1);
  TIM1->CCER |= (1UL << 3);
  //Set pulse width (50%)
  TIM1->CCR1 = 10;

  //Enable Channel1N
  TIM1->CCER |= TIM_CCER_CC1E;
  TIM1->CCER |= TIM_CCER_CC1NE;
  //Slave Controller reset
  TIM1->SMCR = 0;
  //*** Enable MOE output - VERY IMPORTANT FOR TIM1-8 ***//
  TIM1->BDTR |= TIM_BDTR_MOE;
}

/**
 * @brief PWM LED GPIO configuration
 */
void tim_PWM_LED_GPIO_config(void)
{
  //Enable port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  //PB0
  //Set pin mode to Output
  GPIOB->CRL &= ~(3UL << 0);
  GPIOB->CRL |= (2UL << 0);
  //Alternate push-pull
  GPIOB->CRL &= ~(3UL << 2);
  GPIOB->CRL |= (2UL << 2);

  //PB4
  //Set pin mode to Output
  GPIOB->CRL &= ~(3UL << 16);
  GPIOB->CRL |= (2UL << 16);
  //Alternate push-pull
  GPIOB->CRL &= ~(3UL << 18);
  GPIOB->CRL |= (2UL << 18);

  //PB5
  //Set pin mode to Output
  GPIOB->CRL &= ~(3UL << 20);
  GPIOB->CRL |= (2UL << 20);
  //Alternate push-pull
  GPIOB->CRL &= ~(3UL << 22);
  GPIOB->CRL |= (2UL << 22);

  //Remap TIM3 outputs to Partial (CH1-PB4, CH2-PB5, CH3-PB0, CH4-PB1)
  AFIO->MAPR &= ~(3UL << 10);
  AFIO->MAPR |= (2UL << 10);
}

/**
 * @brief PWM configuration
 */
void tim_PWM_LED_config(void)
{
  //Enable TIM3 clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  //Count up
  TIM3->CR1 &= ~(1UL << 4);
  //Disable One pulse
  TIM3->CR1 &= ~(TIM_CR1_OPM);
  //Update request source and update disable to UEV
  TIM3->CR1 &= ~(TIM_CR1_URS | TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM3->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM3->PSC = 0; //72MHz
  //ARR
  TIM3->ARR = 7200-1;   //10KHz
  //Clear Update Event Interrupt
  TIM3->SR &= ~(1UL<<0);

  //Disable OC
  TIM3->CCER=0;
  //CH1
  //Set mode output
  TIM3->CCMR1 &= ~(3UL << 0);
  //PWM Mode
  TIM3->CCMR1 &= ~(7UL << 4);
  TIM3->CCMR1 |= (6UL << 4);
  //Set output polarity to HIGH
  TIM3->CCER &= ~(1UL << 1);
  //Set pulse width
  TIM3->CCR1 = 0;

  //CH2
  //Set mode output
  TIM3->CCMR1 &= ~(3UL << 8);
  //PWM Mode
  TIM3->CCMR1 &= ~(7UL << 12);
  TIM3->CCMR1 |= (6UL << 12);
  //Set output polarity to HIGH
  TIM3->CCER &= ~(1UL << 5);
  //Set pulse width
  TIM3->CCR2 = 0;

  //CH3
  //Set mode output
  TIM3->CCMR2 &= ~(3UL << 0);
  //PWM Mode
  TIM3->CCMR2 &= ~(7UL << 4);
  TIM3->CCMR2 |= (6UL << 4);
  //Set output polarity to HIGH
  TIM3->CCER &= ~(1UL << 9);
  //Set pulse width
  TIM3->CCR3 = 0;

  //Enable Channels
  TIM3->CCER |= (1UL << 0);
  TIM3->CCER |= (1UL << 4);
  TIM3->CCER |= (1UL << 8);
}

/**
 * @brief PWM LED Duty cycle
 */
void tim_PWM_setDutyCycle_CH1(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  TIM3->CCR1 = (uint16_t)(TIM3->ARR + 0.0f)*(duty_0_100/100.0f);
}

void tim_PWM_setDutyCycle_CH2(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  TIM3->CCR2 = (uint16_t)(TIM3->ARR + 0.0f)*(duty_0_100/100.0f);
}

void tim_PWM_setDutyCycle_CH3(uint8_t duty_0_100)
{
  if(duty_0_100>100)duty_0_100=100;
  TIM3->CCR3 = (uint16_t)(TIM3->ARR + 0.0f)*(duty_0_100/100.0f);
}

/**
 * @brief PWM LED RGB
 */
void tim_PWM_setRGB(uint8_t R, uint8_t G, uint8_t B)
{
  TIM3->CCR1 = (uint16_t)(TIM3->ARR + 0.0f)*(R/255.0f);
  TIM3->CCR2 = (uint16_t)(TIM3->ARR + 0.0f)*(G/255.0f);
  TIM3->CCR3 = (uint16_t)(TIM3->ARR + 0.0f)*(B/255.0f);
}

/**
 * @brief Encoder GPIO configuration
 */
void tim_Encoder_GPIO_config(void)
{
  //TIM_IC pin --> PB6
  //Enable port B clock
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  //Set pin mode to Input - PB6
  GPIOB->CRL &= ~(3UL << 24);
  GPIOB->CRL &= ~(3UL << 26);
  GPIOB->CRL |= (1UL << 26);
  //Set pin mode to Input - PB7
  GPIOB->CRL &= ~(3UL << 28);
  GPIOB->CRL &= ~(3UL << 30);
  GPIOB->CRL |= (1UL << 30);
}

/**
 * @brief Encoder configuration
 */
void tim_Encoder_config(void)
{
  //Enable TIM4 clock
  RCC->APB1ENR |= (1UL << 2);
  //Count up
  TIM4->CR1 &= ~(1UL << 4);
  //Disable One pulse
  TIM4->CR1 &= ~(TIM_CR1_OPM);
  //Update request source and update disable to UEV
  TIM4->CR1 &= ~(TIM_CR1_URS | TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM4->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM4->PSC = 0;
  //ARR
  TIM4->ARR = 65535;
  //Clear Update Event Interrupt
  TIM4->SR &= ~(1UL<<0);

  //Reset Slave controller (SMS and ECE)
  TIM4->SMCR &= ~((7UL << 0) | (1UL << 14));
  //Set slave controller to Encoder mode
  TIM4->SMCR |= TIM_SMCR_SMS_0;
  //Set Capture compare as inputs
  TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
  TIM4->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);
  //Capture Compare prescalar to 0
  TIM4->CCMR1 &= ~(TIM_CCMR1_IC1PSC | TIM_CCMR1_IC2PSC);
  //Input filter to 4
  TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);
  TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC2F_1);
  //Set polarity to rising edge
  TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
  TIM4->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC2NP);
  //Start TIM4
  TIM4->CR1 |= TIM_CR1_CEN;
  //Enable the encoder
  TIM4->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);
}

/** SYS TIM-Base Functions **/
bool timBase_initTick(void)
{
  //Enable TIM2 clock
  RCC->APB1ENR |= (1UL << 0);
  //Count up
  TIM2->CR1 &= ~(1UL << 4);
  //Disable one pulse
  TIM2->CR1 &= ~(1UL << 3);
  //Only counter overflow generates event
  TIM2->CR1 |= TIM_CR1_URS;
  //Update Event enabled
  TIM2->CR1 &= ~(TIM_CR1_UDIS);
  //Master mode --> Reset
  TIM2->CR2 &= ~(7UL << 4);
  //Prescaler
  TIM2->PSC = 72-1; //1Mhz
  //ARR
  TIM2->ARR = 1000-1;   //1ms (1KHz)
  //Clear Update Event Interrupt
  TIM2->SR &= ~(1UL<<0);
  //Enable Update interrupt
  TIM2->DIER |= 0x01;
  //Enable NVIC Interrupt
  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM2_IRQn, 5);
  //Start TIM2
  TIM2->CR1 |= 0x01;
  return true;
}

void TIM2_IRQHandler(void)
{
  if(TIM2->SR & 0x01)
  {
    //Reset Interrupt flag
    TIM2->SR &= ~(1UL);
    msIncTick();
  }
}

