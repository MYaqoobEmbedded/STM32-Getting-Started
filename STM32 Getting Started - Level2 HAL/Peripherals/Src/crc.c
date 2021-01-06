/*
 * crc.c
 *
 *  Created on: Dec 10, 2020
 *      Author: Mohamed Yaqoob
 */

#include "crc.h"

CRC_HandleTypeDef hcrc;

/**
 * @brief CRC Configuration
 */
bool crc_config(void)
{
  __HAL_RCC_CRC_CLK_ENABLE();
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    return false;
  }
  return true;
}

