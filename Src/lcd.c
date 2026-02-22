/*
 * i2c_lcd.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#include "lcd.h"
#include "timer2.h"  // For delays
#include "utils.h"

// PCF8574 bits
#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_RW          0x02
#define LCD_RS          0x01

// Send 4 bits to LCD
static void LCD_SendNibble(uint8_t nibble, uint8_t rs)
{
  uint8_t data;

  // Combine nibble with control bits
  data = nibble | LCD_BACKLIGHT;  // Backlight always on

  if(rs)
    data |= LCD_RS;  // Set RS for data

  // ENABLE HIGH
  I2C1_Start();
  I2C1_SendAddr(LCD_ADDR, I2C_WRITE);
  I2C1_WriteByte(data | LCD_ENABLE);  // E=1
  I2C1_Stop();

  // Small pulse delay
  for(volatile int i = 0; i < 10; i++);

  // ENABLE LOW
  I2C1_Start();
  I2C1_SendAddr(LCD_ADDR, I2C_WRITE);
  I2C1_WriteByte(data & ~LCD_ENABLE);  // E=0
  I2C1_Stop();
}

// Send command (RS=0)
void LCD_SendCmd(uint8_t cmd)
{
  // Send high nibble first
  LCD_SendNibble(cmd & 0xF0, 0);

  // Send low nibble
  LCD_SendNibble((cmd << 4) & 0xF0, 0);

  // Commands need execution time
  if(cmd == 0x01 || cmd == 0x02)
    TIMER2_Delay_ms(20);
  else
    TIMER2_Delay_ms(1);
}

// Send data (RS=1)
void LCD_SendData(uint8_t data)
{
  // Send high nibble first
  LCD_SendNibble(data & 0xF0, 1);

  // Send low nibble
  LCD_SendNibble((data << 4) & 0xF0, 1);

  for(volatile int i = 0; i < 500; i++);
}

// Initialize LCD
void LCD_Init(void)
{
  // Power-up delay
  TIMER2_Delay_ms(100);

  // Reset sequence (from HD44780 datasheet)
  LCD_SendNibble(0x30, 0);  // 8-bit mode
  TIMER2_Delay_ms(5);

  LCD_SendNibble(0x30, 0);  // 8-bit mode again
  TIMER2_Delay_ms(5);

  LCD_SendNibble(0x30, 0);  // 8-bit mode again
  TIMER2_Delay_ms(5);

  LCD_SendNibble(0x20, 0);  // Switch to 4-bit mode
  TIMER2_Delay_ms(5);

  // Now in 4-bit mode, send configuration commands
  LCD_SendCmd(0x28);  // 2 lines, 5x8 font
  LCD_SendCmd(0x08);  // Display off
  LCD_SendCmd(0x01);  // Clear display
  LCD_SendCmd(0x06);  // Entry mode
  LCD_SendCmd(0x0C);  // Display on, cursor off
}

// Send string
void LCD_SendString(char *str)
{
  while(*str)
  {
    LCD_SendData(*str++);
  }
}

// Clear display
void LCD_Clear(void)
{
  LCD_SendCmd(0x01);
}

// Set cursor position
void LCD_SetCursor(uint8_t row, uint8_t col)
{
  uint8_t address;

  if(row == 0)
    address = 0x80 + col;
  else
    address = 0xC0 + col;

  LCD_SendCmd(address);
}

void LCD_DisplayReading(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec)
{

  // LINE 1: TEMP: XX.X C
  LCD_SetCursor(0, 0);
  LCD_SendString("TEMP: ");

  // Format temperature: XX.X
  if(temp_int >= 10)
  {
    LCD_SendData('0' + (temp_int / 10));
    LCD_SendData('0' + (temp_int % 10));
  }
  else
  {
    LCD_SendData(' ');
    LCD_SendData('0' + temp_int);
  }

  LCD_SendData('.');
  LCD_SendData('0' + temp_dec);
  LCD_SendData(' ');
  LCD_SendData('C');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');

  // LINE 2: HUMD: XX.X %
  LCD_SetCursor(1, 0);
  LCD_SendString("HUMD: ");

  // Format humidity: XX.X
  if(hum_int >= 10)
  {
    LCD_SendData('0' + (hum_int / 10));
    LCD_SendData('0' + (hum_int % 10));
  }
  else
  {
    LCD_SendData(' ');
    LCD_SendData('0' + hum_int);
  }

  LCD_SendData('.');
  LCD_SendData('0' + hum_dec);
  LCD_SendData(' ');
  LCD_SendData('%');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');

}

void LCD_DisplayError(void)
{
  LCD_Clear();

  LCD_SetCursor(0, 0);
  LCD_SendString("TEMP: ERROR    ");

  LCD_SetCursor(1, 0);
  LCD_SendString("HUMD: ERROR    ");

}

void LCD_DisplayHeader(void)
{
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_SendString("DHT11 Sensor");
  LCD_SetCursor(1, 0);
  LCD_SendString("Initializing...");
}

void LCD_DisplayAccel(int16_t ax, int16_t ay, int16_t az)
{
  char buf[8];

  // Line 1: AX and AY
  LCD_SetCursor(0, 0);
  LCD_SendString("AX:");
  itoa_16(ax, buf);
  LCD_SendString(buf);
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');

  LCD_SetCursor(0, 8);
  LCD_SendString(" AY:");
  itoa_16(ay, buf);
  LCD_SendString(buf);

  // Line 2: AZ
  LCD_SetCursor(1, 0);
  LCD_SendString("AZ:");
  itoa_16(az, buf);
  LCD_SendString(buf);
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
}

void LCD_DisplayGyro(int16_t gx, int16_t gy, int16_t gz)
{
  char buf[8];

  // Line 1: GX and GY
  LCD_SetCursor(0, 0);
  LCD_SendString("GX:");
  itoa_16(gx, buf);
  LCD_SendString(buf);
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');

  LCD_SetCursor(0, 8);
  LCD_SendString(" GY:");
  itoa_16(gy, buf);
  LCD_SendString(buf);

  // Line 2: GZ
  LCD_SetCursor(1, 0);
  LCD_SendString("GZ:");
  itoa_16(gz, buf);
  LCD_SendString(buf);
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
  LCD_SendData(' ');
}
