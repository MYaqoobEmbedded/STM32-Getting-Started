/*
 * rcc.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "rcc.h"

/**
 * @brief RCC System Clock Configuration
 */
bool rcc_systemClockConfig(void)
{
  /*
   * Configuration paramters
   *
   * HSE = 8MHz
   * PLL_M = 9
   * USB prescalar = 1.5
   * AHB prescalar = 1
   * Cortex precalar = 1
   * --> 72MHz System Clock
   *
   * APB1 prescalar = 2  --> 36, 72 MHz
   * APB2 prescalar = 1  --> 36, 36 MHz
   * ADC prescalr = 6    --> 12MHz
   */

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return false;
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  //FLASH_LATENCY_0: 0-24MHz
  //FLASH_LATENCY_1: 24-48MHz
  //FLASH_LATENCY_2: 48-72MHz
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    return false;
  }
  /* Peripherals Clock Initialisation */
  //ADC (Max = 14Mhz, 72MHz source / 6 = 12Mhz)
  PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC |RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    return false;
  }
  return true;
}
