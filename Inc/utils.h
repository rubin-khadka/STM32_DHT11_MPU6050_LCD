/*
 * utils.h
 *
 *  Created on: Feb 20, 2026
 *      Author: Rubin Khadka
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

// Convert 16-bit integer to ASCII string
void itoa_16(int16_t value, char *buffer);

// Convert 8-bit integer to ASCII string
void itoa_8(uint8_t value, char *buffer);

// Format temperature/humidity with one decimal
void format_value(uint8_t integer, uint8_t decimal, char *buffer, char unit);

// Format reading
void format_reading(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec, char *buffer);
void format_temp_hum(char *buffer, uint8_t temp, uint8_t hum);
void format_accel(char *buffer, int16_t ax, int16_t ay, int16_t az);
void format_gyro(char *buffer, int16_t gx, int16_t gy, int16_t gz);

#endif /* UTILS_H_ */
