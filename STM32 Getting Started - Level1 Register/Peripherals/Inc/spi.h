/*
 * spi.h
 *
 *  Created on: 14 Nov 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "main.h"

/**
 * @brief SPI GPIO pins setup
 */
void spi_gpioConfig(void);

/**
 * @brief SPI peripheral configuration
 */
void spi_config(void);

/**
 * @brief SPI Transmit
 */
bool spi_transmit(uint8_t *data, uint8_t len, uint32_t timeout);

/**
 * @brief SPI Receive
 */
bool spi_receive(uint8_t *data, uint8_t len, uint32_t timeout);

/**
 * @brief SPI Receive
 */
bool spi_transmitReceive(uint8_t *dataTx, uint8_t *dataRx, uint8_t len, uint32_t timeout);

/**
 * @brief RFID CS Pin
 */
void spi_cs_rfid_config(void);

/**
 * @brief RFID CS set/reset
 */
void spi_cs_rfid_write(bool state);


/**
 * @brief SD Card SPI-CS
 */
void spi_cs_sd_config(void);

/**
 * @brief SD Card CS pin on/off
 */
void spi_cs_sd_write(bool state);

#endif /* INC_SPI_H_ */
