/*
 * uart.h
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_uart1_rx;
extern DMA_HandleTypeDef hdma_uart1_tx;

/**
 * @brief UART1 GPIO configuration
 */
void uart_UART1_GPIO_config(void);

/**
 * @brief UART1 configuration
 */
bool uart_UART1_config(void);

#endif /* INC_UART_H_ */
