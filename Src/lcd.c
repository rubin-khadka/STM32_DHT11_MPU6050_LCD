/*
 * i2c_lcd.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#include "lcd.h"
#include "timer1.h"  // For delays

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
        for(volatile int i = 0; i < 20000; i++);  // Longer for clear/home
    else
        for(volatile int i = 0; i < 1000; i++);
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
    for(volatile int i = 0; i < 100000; i++);

    // Reset sequence (from HD44780 datasheet)
    LCD_SendNibble(0x30, 0);  // 8-bit mode
    for(volatile int i = 0; i < 20000; i++);

    LCD_SendNibble(0x30, 0);  // 8-bit mode again
    for(volatile int i = 0; i < 5000; i++);

    LCD_SendNibble(0x30, 0);  // 8-bit mode again
    for(volatile int i = 0; i < 5000; i++);

    LCD_SendNibble(0x20, 0);  // Switch to 4-bit mode
    for(volatile int i = 0; i < 5000; i++);

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
