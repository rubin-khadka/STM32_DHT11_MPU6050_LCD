/*
 * timer2.c
 *
 *  Created on: Feb 20, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "timer2.h"

void TIMER2_Init(void)
{
    // Enable TIM2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configure for 1ms resolution at 72MHz
    TIM2->PSC = 72000 - 1;  // Prescaler = 71999
    TIM2->ARR = 0xFFFFFFFF;  // 32-bit max (no overflow)

    // Disable interrupt
    TIM2->DIER &= ~TIM_DIER_UIE;

    // Start timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIMER2_Delay_ms(uint32_t ms)
{
    uint32_t start = TIM2->CNT;

    // Wait for desired milliseconds
    while((TIM2->CNT - start) < ms);
}
