#include "main.h"
#include "rcc.h"
#include "uart.h"
#include "gpio.h"
#include "exti.h"
#include "adc.h"
#include "tim.h"
#include <time.h>
#include "rtc.h"
#include "spi.h"
#include "rc522.h"
#include "i2c.h"
#include "TJ_MPU6050.h"
#include "crc.h"
#include "pwr.h"
#include "flash.h"
#include "fatfs.h"
#include "cmsis_os.h"

//Thread1
osThreadId Thread1Handle;
void Thread1Handler(void const * argument);

int main(void)
{
  //HSE Configuration
  rcc_HSE_config();
  rcc_SysTick_config(72000);
  //UART Configuration
  uart_UART1_GPIO_config();
  uart_UART1_config();
  //LED Configuration
  gpio_LED_config();
  gpio_LED_writeRed(1);
  //Button
  gpio_PB_config();
  //Switches
  gpio_SW_config();

  //FreeRTOS Init
  osThreadDef(Thread1, Thread1Handler, osPriorityNormal, 0, 128);
  Thread1Handle = osThreadCreate(osThread(Thread1), NULL);

  /* Start scheduler */
  osKernelStart();

  while(1)
  {

  }
}

//Thread1
void Thread1Handler(void const * argument)
{
  while(1)
  {
    gpio_LED_toggleRed();
    printf("LED Blinking!\n");
    osDelay(1000);
  }
}

