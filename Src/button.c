/*
 * button.c
 *
 *  Created on: Feb 22, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "button.h"
#include "uart.h"

// Current display mode
static volatile DisplayMode_t current_mode = DISPLAY_MODE_TEMP_HUM;

void Button_Init(void)
{
  // Enable Clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // GPIO Configuration for button
  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
  GPIOA->CRL |= GPIO_CRL_CNF0_1;  // Input mode push-pull
  GPIOA->ODR |= GPIO_ODR_ODR0;    // GPIO pull-up

  // Connect PA0 to External Interrupt 0
  AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;
  AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;

  // Disable interrupt while configuring
  EXTI->IMR &= ~EXTI_IMR_MR0;

  // Configure trigger edge
  EXTI->FTSR |= EXTI_FTSR_TR0;   // Enable falling edge trigger
  EXTI->RTSR &= ~EXTI_RTSR_TR0;  // Disable rising edge

  // Clear any pending interrupt
  EXTI->PR |= EXTI_PR_PR0;

  // Enable interrupt
  EXTI->IMR |= EXTI_IMR_MR0;

  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn, 0);  // Highest priority
}

DisplayMode_t Button_GetMode(void)
{
  return current_mode;
}

// Change to next mode
void Button_NextMode(void)
{
  current_mode++;
  if(current_mode >= DISPLAY_MODE_COUNT)
  {
    current_mode = DISPLAY_MODE_TEMP_HUM;
  }

  // Debug message
  USART1_SendString("Mode changed to: ");
  switch(current_mode)
  {
    case DISPLAY_MODE_TEMP_HUM:
      USART1_SendString("Temperature/Humidity\r\n");
      break;
    case DISPLAY_MODE_ACCEL:
      USART1_SendString("Accelerometer\r\n");
      break;
    case DISPLAY_MODE_GYRO:
      USART1_SendString("Gyroscope\r\n");
      break;
    default:
      break;
  }
}

// EXTI0 Interrupt Handler
void EXTI0_IRQHandler(void)
{
  // Check if EXTI0 triggered
  if(EXTI->PR & EXTI_PR_PR0)
  {
    // Disable EXTI0 temporarily
    EXTI->IMR &= ~EXTI_IMR_MR0;

    // For now, just change mode immediately
    Button_NextMode();

    // Clear the interrupt flag
    EXTI->PR |= EXTI_PR_PR0;

    // Re-enable EXTI0 (temporary - will be moved to timer)
    EXTI->IMR |= EXTI_IMR_MR0;
  }
}
