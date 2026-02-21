/*
 * i2c1.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "i2c1.h"

void I2C1_Init(void)
{
  // Enable Clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Enable I2C1 Remap to PB8PB9
  AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;

  // GPIO Configuration for I2C1
  // PB8 SCL
  GPIOB->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8);
  GPIOB->CRH |= GPIO_CRH_MODE8_1;	// 2 MHz
  GPIOB->CRH |= GPIO_CRH_CNF8_0 | GPIO_CRH_CNF8_1;	// Alternate function open drain

  // PB9 SDA
  GPIOB->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
  GPIOB->CRH |= GPIO_CRH_MODE9_1;	// 2 MHz
  GPIOB->CRH |= GPIO_CRH_CNF9_0 | GPIO_CRH_CNF9_1;	// Alternate function open drain

  // I2C Configuration
  // Reset I2C
  I2C1->CR1 |= I2C_CR1_SWRST;
  for(volatile int i = 0; i < 100; i++);
  I2C1->CR1 &= ~I2C_CR1_SWRST;

  // APB1 Clock Speed
  I2C1->CR2 |= 36;

  // Configure for 100kHz I2C
  I2C1->CCR = 180;        // 36MHz / 2 / 100kHz = 180
  I2C1->TRISE = 37;       // Rise time for 100kHz

  // Set duty cycle (standard for 100kHz)
  I2C1->CCR &= ~I2C_CCR_DUTY;  // DUTY=0 for standard mode

  // Disable general call, no stretch, dual address disable
  I2C1->CR1 &= ~(I2C_CR1_ENGC | I2C_CR1_NOSTRETCH | I2C_SR2_DUALF);

  // Set own address to 0
  I2C1->OAR1 = 0x4000;  // 7-bit mode, address 0

  // Enable I2C with ACK
  I2C1->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
}

void I2C1_Start(void)
{
  uint32_t timeout = 10000;
  while(I2C1->SR2 & I2C_SR2_BUSY)
  {
    if(--timeout == 0)
      break;
  }

  // Generate start condition
  I2C1->CR1 |= I2C_CR1_START;

  while(!(I2C1->SR1 & I2C_SR1_SB))
  {
    if(--timeout == 0)
      break;
  }
}

void I2C1_Stop(void)
{
  // Generate stop condition
  I2C1->CR1 |= I2C_CR1_STOP;

  // Small delay to let STOP complete
  for(volatile int i = 0; i < 100; i++);
}

uint8_t I2C1_SendAddr(uint8_t addr, uint8_t rw)
{
  uint32_t timeout = 10000;

  // Clear any pending acknowledge failure
  I2C1->SR1 &= ~I2C_SR1_AF;

  // Send address (shifted left 1 bit + R/W bit)
  I2C1->DR = (addr << 1) | rw;

  // Wait for ADDR flag (address sent and ACK received)
  while(!(I2C1->SR1 & I2C_SR1_ADDR))
  {
    // Check for acknowledge failure
    if(I2C1->SR1 & I2C_SR1_AF)
    {
      return I2C_ERROR;  // No ACK from device
    }

    if(--timeout == 0)
    {
      return I2C_TIMEOUT;
    }
  }

  // Clear ADDR flag by reading SR2 register
  (void) I2C1->SR2;

  return I2C_OK;
}

uint8_t I2C1_WriteByte(uint8_t data)
{
  uint32_t timeout = 10000;

  // Wait for TXE flag (data register empty)
  while(!(I2C1->SR1 & I2C_SR1_TXE))
  {
    // Check for acknowledge failure
    if(I2C1->SR1 & I2C_SR1_AF)
    {
      return I2C_ERROR;
    }

    if(--timeout == 0)
    {
      return I2C_TIMEOUT;
    }
  }

  // Send data
  I2C1->DR = data;

  // Wait for byte to be transmitted (TXE set again)
  timeout = 10000;
  while(!(I2C1->SR1 & I2C_SR1_TXE))
  {
    // Check for acknowledge failure
    if(I2C1->SR1 & I2C_SR1_AF)
    {
      return I2C_ERROR;
    }

    if(--timeout == 0)
    {
      return I2C_TIMEOUT;
    }
  }

  return I2C_OK;
}

uint8_t I2C1_ReadByte(uint8_t ack)
{
  uint32_t timeout = 10000;

  // Configure ACK/NACK for this byte
  if(ack)
  {
    I2C1->CR1 |= I2C_CR1_ACK;   // Send ACK after receiving
  }
  else
  {
    I2C1->CR1 &= ~I2C_CR1_ACK;  // Send NACK after receiving
  }

  // Wait for RXNE flag (data received)
  while(!(I2C1->SR1 & I2C_SR1_RXNE))
  {
    if(--timeout == 0)
    {
      return 0;
    }
  }

  // Read received data
  return I2C1->DR;
}

uint8_t I2C1_WaitForEvent(uint32_t event_mask, uint32_t timeout)
{
  while(!(I2C1->SR1 & event_mask))
  {
    if(--timeout == 0)
    {
      return I2C_TIMEOUT;
    }
  }
  return I2C_OK;
}

