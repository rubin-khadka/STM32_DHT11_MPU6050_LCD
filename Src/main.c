/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Rubin Khadka
 * @brief          : STM32 DHT11 UART Project main file
 ******************************************************************************
 */

#include "stm32f103xb.h"
#include "timer1.h"   // For DHT11 microsecond timing
#include "timer2.h"   // For long delays between readings
#include "dht11.h"
#include "utils.h"
#include "lcd.h"
#include "mpu6050.h"
#include "button.h"
#include "uart.h"
#include "tasks.h"
#include "timer3.h"

int main(void)
{

  // Initialize ALL modules
  TIMER1_Init();    // 1µs resolution for DHT11
  TIMER2_Init();    // 0.1ms resolution for long delays
  USART1_Init();    // UART for output
  DHT11_Init();     // DHT11 sensor on PB0
  I2C1_Init();
  LCD_Init();       // LCD for display
  MPU6050_Init();
  Button_Init();
  TIMER4_Init();

  // Loop counters
  uint8_t dht_count = 0;
  uint8_t mpu_count = 0;
  uint8_t lcd_count = 0;
  uint8_t uart_count = 0;

  LCD_Clear();
  LCD_SendString("STM32 PROJECT");
  LCD_SetCursor(1, 0);
  LCD_SendString("INITIALIZING...");

  TIMER2_Delay_ms(2000);

  // Setup TIM3 for 10ms control loop
  TIMER3_SetupPeriod(10);  // 10ms period

  while(1)
  {
    // Run tasks at different rates

    // DHT11 every 1 seconds
    if(dht_count++ >= 100)
    {
      Task_DHT11_Read();
      dht_count = 0;
    }

    // MPU6050 every 50ms
    if(mpu_count++ >= 5)
    {
      Task_MPU6050_Read();
      mpu_count = 0;
    }

    // LCD update every 100ms
    if(lcd_count++ >= 10)
    {
      Task_LCD_Update();
      lcd_count = 0;
    }

    // UART output every 100ms
    if(uart_count++ >= 10)
    {
      Task_UART_Output();
      uart_count = 0;
    }

    TIMER3_WaitPeriod();
  }
}
