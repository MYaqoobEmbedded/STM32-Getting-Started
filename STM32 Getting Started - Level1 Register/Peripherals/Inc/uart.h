/*
 * uart.h
 *
 *  Created on: 3 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

/**
 * @brief UART1 GPIO configuration
 */
void uart_UART1_GPIO_config(void);

/**
 * @brief UART1 configuration
 */
void uart_UART1_config(void);

/**
 * @brief UART1 transmit
 */
bool uart_UART1_transmit(uint8_t *data, uint8_t len, uint32_t timeout);

/**
 * @brief UART1 receive
 */
bool uart_UART1_receive(uint8_t *data, uint8_t len, uint32_t timeout);

/**
 * @brief UART1 Receive DMA
 */
void uart_UART1_DMA_config(void);

/**
 * @brief UART1 DMA Transmit
 */
void uart_UART1_DMA_transmit(uint8_t *data, uint8_t len);

/**
 * @brief UART1 DMA Receive
 */
void uart_UART1_DMA_receive(uint8_t *data, uint8_t len);


#endif /* INC_UART_H_ */
