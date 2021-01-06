/*
 * main.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Mohamed Yaqoob
 */

#include "main.h"
#include "rcc.h"
#include "uart.h"
#include "gpio.h"
#include "exti.h"
#include "adc.h"
#include "tim.h"
#include "rtc.h"
#include "spi.h"
#include "rc522.h"
#include "i2c.h"
#include "TJ_MPU6050.h"
#include "crc.h"
#include "pwr.h"
#include "fatfs.h"
#include "usb_device.h"

int main(void)
{
  HAL_Init();
  rcc_systemClockConfig();
  //UART1-Printf
  uart_UART1_GPIO_config();
  uart_UART1_config();
  //LEDs
  gpio_LED_config();
  //Push-button
  gpio_PB_config();
  //Switches
  gpio_SW_config();
  //SD Card
  spi_GPIO_config();
  spi_config();

  //USB Initialisation
  MX_USB_DEVICE_Init();

  while(1)
  {
    gpio_LED_toggleGreen();
    HAL_Delay(500);
  }
}

