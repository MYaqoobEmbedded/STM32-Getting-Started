/*
 * adc.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "adc.h"

ADC_HandleTypeDef adc1Handle;

/**
 * @brief ADC channels GPIO configuration
 */
void adc_GPIO_config(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  //PA1, PA2, PA3
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief ADC single channel configuration
 */
bool adc_singleConfig(ADC_SingleSelect_e channel)
{
  __HAL_RCC_ADC1_CLK_ENABLE();
  ADC_ChannelConfTypeDef   sConfig;
  //ADC Initialisation
  adc1Handle.Instance = ADC1;
  adc1Handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  adc1Handle.Init.ScanConvMode          = ADC_SCAN_DISABLE;
  adc1Handle.Init.ContinuousConvMode    = DISABLE;
  adc1Handle.Init.NbrOfConversion       = 1;
  adc1Handle.Init.DiscontinuousConvMode = DISABLE;
  adc1Handle.Init.NbrOfDiscConversion   = 1;
  adc1Handle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  if (HAL_ADC_Init(&adc1Handle) != HAL_OK)
  {
    return false;
  }
  //ADC Channel configuration
  //Rank1
  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if(HAL_ADC_ConfigChannel(&adc1Handle, &sConfig) != HAL_OK)
  {
    return false;
  }
  return true;
}

/**
 * @brief ADC Interrupt
 */
void adc_interruptConfig(void)
{
  __HAL_ADC_ENABLE_IT(&adc1Handle, ADC_IT_EOC);
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

/**
 * @brief ADC Multi-Channel Config
 */
bool adc_multiChannelConfig(void)
{
  //Enable ADC Clock
  __HAL_RCC_ADC1_CLK_ENABLE();
  ADC_ChannelConfTypeDef   sConfig;
  //ADC Initialisation
  adc1Handle.Instance = ADC1;
  adc1Handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  adc1Handle.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  adc1Handle.Init.ContinuousConvMode    = DISABLE;
  adc1Handle.Init.NbrOfConversion       = 3;
  adc1Handle.Init.DiscontinuousConvMode = DISABLE;
  adc1Handle.Init.NbrOfDiscConversion   = 1;
  adc1Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  if (HAL_ADC_Init(&adc1Handle) != HAL_OK)
  {
    return false;
  }
  //ADC Channel configuration
  //Rank1 - PA1
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if(HAL_ADC_ConfigChannel(&adc1Handle, &sConfig) != HAL_OK)
  {
    return false;
  }
  //Rank2 - PA2
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if(HAL_ADC_ConfigChannel(&adc1Handle, &sConfig) != HAL_OK)
  {
    return false;
  }
  //Rank3 - PA3
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 3;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if(HAL_ADC_ConfigChannel(&adc1Handle, &sConfig) != HAL_OK)
  {
    return false;
  }
  return true;
}

/**
 * @brief ADC Multi-Channel DMA Configuration
 */
void adc_dmaConfig(void)
{
  static DMA_HandleTypeDef  DmaHandle;
  __HAL_RCC_DMA1_CLK_ENABLE();

  DmaHandle.Instance                 = DMA1_Channel1;
  DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  DmaHandle.Init.Mode                = DMA_CIRCULAR;
  DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;
  HAL_DMA_Init(&DmaHandle);
  __HAL_LINKDMA(&adc1Handle, DMA_Handle, DmaHandle);
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
 * @brief ADC Analog Watchdog configuration
 */
bool adc_awdgConfig(ADC_SingleSelect_e channel)
{
  ADC_AnalogWDGConfTypeDef awdgConfig = {0};
  awdgConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
  awdgConfig.HighThreshold = 4000;
  awdgConfig.LowThreshold = 1000;
  awdgConfig.Channel = channel;
  awdgConfig.ITMode = ENABLE;
  if (HAL_ADC_AnalogWDGConfig(&adc1Handle, &awdgConfig) != HAL_OK)
  {
    return false;
  }
  __HAL_ADC_ENABLE_IT(&adc1Handle, ADC_IT_AWD);
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

  return true;
}

/**
 * @brief ADC Injected Channel configuration
 */
bool adc_injectedConfig(void)
{
  ADC_InjectionConfTypeDef sConfigInjected = {0};
  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_3;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.AutoInjectedConv = ENABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&adc1Handle, &sConfigInjected) != HAL_OK)
  {
    return false;
  }
  return true;
}

