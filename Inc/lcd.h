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

// LCD I2C address (8-bit from HAL = 0x4E, so 7-bit = 0x27)
#define LCD_ADDR 0x27

void LCD_Init(void);
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

#endif /* LCD_H_ */
