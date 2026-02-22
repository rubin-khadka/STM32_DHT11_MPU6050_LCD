/*
 * mpu6050.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#include "mpu6050.h"
#include "i2c1.h"
#include "uart.h"
#include "timer2.h"

volatile MPU6050_Data_t mpu6050_data;

// Read a single register from MPU6050
static uint8_t MPU6050_ReadReg(uint8_t reg, uint8_t *data)
{
  I2C1_Start();
  if(I2C1_SendAddr(MPU6050_ADDR, I2C_WRITE) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  if(I2C1_WriteByte(reg) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  I2C1_Start();  // Repeated start
  if(I2C1_SendAddr(MPU6050_ADDR, I2C_READ) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  *data = I2C1_ReadByte(0);  // NACK on last byte
  I2C1_Stop();

  return I2C_OK;
}

// Write a single register to MPU6050
static uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
  I2C1_Start();
  if(I2C1_SendAddr(MPU6050_ADDR, I2C_WRITE) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  if(I2C1_WriteByte(reg) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  if(I2C1_WriteByte(data) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  I2C1_Stop();
  return I2C_OK;
}

// Initialize MPU6050
uint8_t MPU6050_Init(void)
{
  uint8_t who_am_i;

  // Check if device is present
  if(MPU6050_ReadReg(MPU6050_WHO_AM_I, &who_am_i) != I2C_OK)
  {
    USART1_SendString("Failed to read WHO_AM_I\r\n");
    return I2C_ERROR;
  }

  if(who_am_i != 0x68)
  {
    USART1_SendString("Wrong device ID!\r\n");
    return I2C_ERROR;
  }

  // Wake up MPU6050 (clear sleep bit)
  if(MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00) != I2C_OK)
  {
    USART1_SendString("Failed to wake device\r\n");
    return I2C_ERROR;
  }

  USART1_SendString("MPU6050 initialized successfully!\r\n");

  TIMER2_Delay_ms(10);

  return I2C_OK;
}

// Read multiple bytes from MPU6050 (burst read)
static uint8_t MPU6050_ReadBurst(uint8_t start_reg, uint8_t *data, uint8_t len)
{
  I2C1_Start();
  if(I2C1_SendAddr(MPU6050_ADDR, I2C_WRITE) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  if(I2C1_WriteByte(start_reg) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  I2C1_Start();  // Repeated start
  if(I2C1_SendAddr(MPU6050_ADDR, I2C_READ) != I2C_OK)
  {
    I2C1_Stop();
    return I2C_ERROR;
  }

  for(uint8_t i = 0; i < len; i++)
  {
    // Send ACK for all bytes except the last one
    uint8_t ack = (i < (len - 1)) ? 1 : 0;
    data[i] = I2C1_ReadByte(ack);
  }

  I2C1_Stop();
  return I2C_OK;
}

// Read all sensor data (accelerometer, temperature, gyroscope)
uint8_t MPU6050_ReadAll(MPU6050_Data_t *data)
{
  uint8_t buffer[14];  // 7 measurements × 2 bytes each

  // Read all 14 bytes starting from ACCEL_XOUT_H (0x3B)
  if(MPU6050_ReadBurst(MPU6050_ACCEL_XOUT_H, buffer, 14) != I2C_OK)
  {
    return I2C_ERROR;
  }

  // Combine high and low bytes for each measurement
  data->accel_x = (buffer[0] << 8) | buffer[1];
  data->accel_y = (buffer[2] << 8) | buffer[3];
  data->accel_z = (buffer[4] << 8) | buffer[5];
  data->temp = (buffer[6] << 8) | buffer[7];
  data->gyro_x = (buffer[8] << 8) | buffer[9];
  data->gyro_y = (buffer[10] << 8) | buffer[11];
  data->gyro_z = (buffer[12] << 8) | buffer[13];

  return I2C_OK;
}

// Read only accelerometer data
uint8_t MPU6050_ReadAccel(MPU6050_Data_t *data)
{
  uint8_t buffer[6];

  if(MPU6050_ReadBurst(MPU6050_ACCEL_XOUT_H, buffer, 6) != I2C_OK)
  {
    return I2C_ERROR;
  }

  data->accel_x = (buffer[0] << 8) | buffer[1];
  data->accel_y = (buffer[2] << 8) | buffer[3];
  data->accel_z = (buffer[4] << 8) | buffer[5];

  return I2C_OK;
}

// Read only gyroscope data
uint8_t MPU6050_ReadGyro(MPU6050_Data_t *data)
{
  uint8_t buffer[6];

  if(MPU6050_ReadBurst(MPU6050_GYRO_XOUT_H, buffer, 6) != I2C_OK)
  {
    return I2C_ERROR;
  }

  data->gyro_x = (buffer[0] << 8) | buffer[1];
  data->gyro_y = (buffer[2] << 8) | buffer[3];
  data->gyro_z = (buffer[4] << 8) | buffer[5];

  return I2C_OK;
}

// Read temperature only
uint8_t MPU6050_ReadTemp(MPU6050_Data_t *data)
{
  uint8_t buffer[2];

  if(MPU6050_ReadBurst(MPU6050_TEMP_OUT_H, buffer, 2) != I2C_OK)
  {
    return I2C_ERROR;
  }

  data->temp = (buffer[0] << 8) | buffer[1];

  return I2C_OK;
}

// Convert raw temperature to degrees Celsius
float MPU6050_ConvertTemp(int16_t raw_temp)
{
  // Formula from datasheet: Temperature = (raw_temp / 340.0) + 36.53
  return (raw_temp / 340.0f) + 36.53f;
}

// Convert raw accelerometer to g (assuming ±2g range)
float MPU6050_ConvertAccel(int16_t raw_accel)
{
  // For ±2g range: 16384 LSB/g
  return raw_accel / 16384.0f;
}

// Convert raw gyroscope to degrees/sec (assuming ±250°/s range)
float MPU6050_ConvertGyro(int16_t raw_gyro)
{
  // For ±250°/s range: 131 LSB/°/s
  return raw_gyro / 131.0f;
}
