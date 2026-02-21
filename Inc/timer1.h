/**
  * @file    timer1.h
  * @brief   Timer1 module with polling-based delays
  * @author  Rubin Khadka
  */

#ifndef TIMER1_H
#define TIMER1_H

#include "stdint.h"

void TIMER1_Init(void);
void TIMER1_Delay_ms(uint32_t ms_delay);
void TIMER1_Delay_us(uint32_t us);

#endif
