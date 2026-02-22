/*
 * lcd.h
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#ifndef LCD_H_
#define LCD_H_

#include "stm32f103xb.h"
#include "i2c1.h"

// LCD I2C address (8 bit address 0x4E)
#define LCD_ADDR 0x27

void LCD_Init(void);
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_DisplayReading(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec);
void LCD_DisplayError(void);
void LCD_DisplayHeader(void);
void LCD_DisplayGyro(int16_t gx, int16_t gy, int16_t gz);
void LCD_DisplayAccel(int16_t ax, int16_t ay, int16_t az);

#endif /* LCD_H_ */
