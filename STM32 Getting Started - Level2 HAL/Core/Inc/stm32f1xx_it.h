/*
 * stm32f1xx_it.h
 *
 *  Created on: Dec 6, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_STM32F1XX_IT_H_
#define INC_STM32F1XX_IT_H_

void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void ADC1_2_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);
void TIM3_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void USART1_IRQHandler(void);
void USB_HP_CAN1_TX_IRQHandler(void);
void USB_LP_CAN1_RX0_IRQHandler(void);

#endif /* INC_STM32F1XX_IT_H_ */
