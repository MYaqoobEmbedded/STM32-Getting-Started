/*
 * stm32f1xx_it.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "main.h"
#include "stm32f1xx_it.h"
#include "adc.h"
#include "gpio.h"
#include "tim.h"
#include "rtc.h"
#include "uart.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief ADC EOC Interrupt handling
 */
void ADC1_2_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&adc1Handle);
}

/**
 * @brief DMA Interrupt Handler
 */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(adc1Handle.DMA_Handle);
}

/**
 * @brief TIM3 Interrupt Handler
 */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

/**
 * @brief RTC Alarm Interrupt Handler
 */
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&hrtc);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart1_tx);
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart1_rx);
}

/**
  * @brief This function handles USB high priority or CAN TX interrupts.
  */
void USB_HP_CAN1_TX_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

