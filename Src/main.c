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

  LCD_SendString("Button Test");
  LCD_SetCursor(1, 0);
  LCD_SendString("Mode: Temp/Hum");

  while(1)
  {
    // Just show current mode on LCD
    LCD_SetCursor(1, 6);  // Position after "Mode: "

    switch(Button_GetMode())
    {
      case DISPLAY_MODE_TEMP_HUM:
        Task_DHT11_Read();
        Task_LCD_Update();
        Task_UART_Output();
        break;
      case DISPLAY_MODE_ACCEL:
        Task_MPU6050_Read();
        Task_LCD_Update();
        Task_UART_Output();
        break;
      case DISPLAY_MODE_GYRO:
        Task_MPU6050_Read();
        Task_LCD_Update();
        Task_UART_Output();
        break;
      default:
        break;
    }

    // Small delay to prevent LCD flicker
    for(volatile uint32_t i = 0; i < 720000; i++);
  }
}
