/**
 * @file    timer1.c
 * @brief   Timer1 module with polling-based delays
 * @author  Rubin Khadka
 */

#include "stm32f103xb.h"
#include "timer1.h"

void TIMER1_Init(void)
{
	// Enable timer 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	// Configure for 1 microsecond base
	TIM1->PSC = 71;

	// Disable interrupt
	TIM1->DIER &= ~TIM_DIER_UIE;

	// Stop timer initially
	TIM1->CR1 &= ~TIM_CR1_CEN;
}

void TIMER1_Delay_ms(uint32_t ms_delay)
{
	uint32_t ticks = ms_delay * 1000;

	TIMER1_Delay_us(ticks);
}

void TIMER1_Delay_us(uint32_t us)
{
	// Safety: prevent overflow if us > 65535
	if(us > 65535)
	{
		us = 65535;  // or handle in a loop
	}

	// Stop timer if running
	TIM1->CR1 &= ~TIM_CR1_CEN;

	// Set auto-reload value
	TIM1->ARR = (uint16_t)(us - 1);

	// Reset counter
	TIM1->CNT = 0;

	// Start timer
	TIM1->CR1 |= TIM_CR1_CEN;

	// Wait until counter reaches ARR
	while(!(TIM1->SR & TIM_SR_UIF));

	// Clear update flag
	TIM1->SR &= ~TIM_SR_UIF;

	// Stop timer
	TIM1->CR1 &= ~TIM_CR1_CEN;
}
