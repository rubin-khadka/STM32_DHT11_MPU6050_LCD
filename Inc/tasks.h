/*
 * tasks.h
 *
 *  Created on: Feb 22, 2026
 *      Author: Rubin Khadka
 */

#ifndef TASKS_H_
#define TASKS_H_

extern volatile uint8_t dht11_humidity, dht11_humidity2, dht11_temperature, dht11_temperature2;

// Task function prototypes
void Task_DHT11_Read(void);
void Task_MPU6050_Read(void);
void Task_LCD_Update(void);
void Task_UART_Output(void);

#endif /* TASKS_H_ */
