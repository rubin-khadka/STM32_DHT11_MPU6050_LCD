/*
 * i2c1.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"

void I2C1_Init(void)
{
	// Enable Clocks
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// GPIO Configuration for I2C1

}
