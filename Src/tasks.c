/*
 * tasks.c
 *
 *  Created on: Feb 22, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "timer2.h"
#include "dht11.h"
#include "uart.h"
#include "button.h"
#include "utils.h"
#include "mpu6050.h"
#include "i2c1.h"
#include "lcd.h"

#define MAX_RETRIES 5

// Global variables to store DHT11 data
volatile uint8_t dht11_humidity = 0;
volatile uint8_t dht11_humidity2 = 0;
volatile uint8_t dht11_temperature = 0;
volatile uint8_t dht11_temperature2 = 0;

static char uart_buf[32];

void Task_DHT11_Read(void)
{
  uint8_t hum1, hum2, temp1, temp2, checksum;

  // Disable interrupts for critical section
  uint32_t primask = __get_PRIMASK();
  __disable_irq();

  // Try up to MAX_RETRIES times
  for(int retry = 0; retry < MAX_RETRIES; retry++)
  {
    DHT11_Start();

    if(DHT11_Check_Response())
    {
      hum1 = DHT11_Read();
      hum2 = DHT11_Read();
      temp1 = DHT11_Read();
      temp2 = DHT11_Read();
      checksum = DHT11_Read();

      uint8_t calc = hum1 + hum2 + temp1 + temp2;

      if(calc == checksum)
      {
        dht11_humidity = hum1;
        dht11_humidity2 = hum2;
        dht11_temperature = temp1;
        dht11_temperature2 = temp2;
        break;
      }
    }
    TIMER2_Delay_ms(1);
  }

  // Re-enable interrupts
  __set_PRIMASK(primask);
}

void Task_UART_Output(void)
{
  DisplayMode_t mode = Button_GetMode();

  switch(mode)
  {
    case DISPLAY_MODE_TEMP_HUM:
      format_reading(dht11_temperature, dht11_temperature2, dht11_humidity, dht11_humidity2, uart_buf);
      break;

    case DISPLAY_MODE_ACCEL:
      format_accel(uart_buf, mpu6050_data.accel_x, mpu6050_data.accel_y, mpu6050_data.accel_z);
      break;

    case DISPLAY_MODE_GYRO:
      format_gyro(uart_buf, mpu6050_data.gyro_x, mpu6050_data.gyro_y, mpu6050_data.gyro_z);
      break;

    default:
      return;
  }

  USART1_SendString(uart_buf);
}

void Task_MPU6050_Read(void)
{
  MPU6050_Data_t temp_data;

  if(MPU6050_ReadAll(&temp_data) == I2C_OK)
  {
    mpu6050_data.accel_x = temp_data.accel_x;
    mpu6050_data.accel_y = temp_data.accel_y;
    mpu6050_data.accel_z = temp_data.accel_z;
    mpu6050_data.temp = temp_data.temp;
    mpu6050_data.gyro_x = temp_data.gyro_x;
    mpu6050_data.gyro_y = temp_data.gyro_y;
    mpu6050_data.gyro_z = temp_data.gyro_z;
  }
}

void Task_LCD_Update(void)
{
  DisplayMode_t mode = Button_GetMode();

  switch(mode)
  {
    case DISPLAY_MODE_TEMP_HUM:
      LCD_DisplayReading(dht11_temperature, dht11_temperature2, dht11_humidity, dht11_humidity2);
      break;

    case DISPLAY_MODE_ACCEL:
      LCD_DisplayAccel(mpu6050_data.accel_x, mpu6050_data.accel_y, mpu6050_data.accel_z);
      break;

    case DISPLAY_MODE_GYRO:
      LCD_DisplayGyro(mpu6050_data.gyro_x, mpu6050_data.gyro_y, mpu6050_data.gyro_z);
      break;
    default:  // Handles DISPLAY_MODE_COUNT and any invalid values
      break;
  }
}
