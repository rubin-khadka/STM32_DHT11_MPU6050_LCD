/*
 * i2c1.h
 *
 *  Created on: Feb 21, 2026
 *      Author: Rubin Khadka
 */

#ifndef I2C1_H_
#define I2C1_H_

// I2C Status codes
#define I2C_OK       	0
#define I2C_ERROR    	-1
#define I2C_TIMEOUT   -2

// I2C Read/Write flags
#define I2C_WRITE       0
#define I2C_READ        1

// Function prototypes
void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
uint8_t I2C1_SendAddr(uint8_t addr, uint8_t rw);
uint8_t I2C1_WriteByte(uint8_t data);
uint8_t I2C1_ReadByte(uint8_t ack);
uint8_t I2C1_WaitForEvent(uint32_t event_mask, uint32_t timeout);

#endif /* I2C1_H_ */
