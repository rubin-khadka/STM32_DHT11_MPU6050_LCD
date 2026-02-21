/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Rubin Khadka
 * @brief          : LCD Test with I2C
 ******************************************************************************
 */

#include "stm32f103xb.h"
#include "i2c1.h"
#include "lcd.h"
#include "timer1.h"

int main(void)
{
    // Initialize
    TIMER1_Init();
    I2C1_Init();

    // Initialize LCD
    LCD_Init();

    // Test LCD
    LCD_SendString("Hello World!");
    LCD_SetCursor(1, 0);
    LCD_SendString("STM32 BareMetal");

    while(1)
    {
        // Blink LED
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        for(volatile uint32_t i = 0; i < 7200000; i++);
    }
}
