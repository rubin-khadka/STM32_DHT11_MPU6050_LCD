/*
 * system_stm32f1xx.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "stdint.h"

// System Core Clock Frequency
uint32_t SystemCoreClock = 72000000;

void SystemInit(void)
{
  // Enable HSE (8MHz external crystal)
  RCC->CR |= RCC_CR_HSEON;
  while((RCC->CR & RCC_CR_HSERDY) == 0);

  // Enable HSI
  RCC->CR |= RCC_CR_HSION;

  // Configure Flash for 72MHz (2 wait states)
  FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_PRFTBE;

  // Configure AHB, APB1, APB2 prescalers FIRST
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;      // AHB = SYSCLK (72MHz)
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;     // APB1 = HCLK/2 (36MHz)
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;     // APB2 = HCLK (72MHz)

  // Configure PLL: 8MHz × 9 = 72MHz
  RCC->CFGR &= ~(RCC_CFGR_PLLMULL | RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE);
  RCC->CFGR |= RCC_CFGR_PLLMULL9;        // Multiply by 9
  RCC->CFGR |= RCC_CFGR_PLLSRC;          // Use HSE as source

  // Enable PLL and wait for lock
  RCC->CR |= RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0);

  // Switch system clock to PLL
  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  // Optional: Enable Clock Security System
  RCC->CR |= RCC_CR_CSSON;

  // Update SystemCoreClock
  SystemCoreClock = 72000000;
}
