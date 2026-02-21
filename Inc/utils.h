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
void itoa_16(int16_t value, char* buffer);

// Convert 8-bit integer to ASCII string
void itoa_8(uint8_t value, char* buffer);

// Format temperature/humidity with one decimal
void format_value(uint8_t integer, uint8_t decimal, char* buffer, char unit);

// Format reading with count, temp, humidity
void format_reading(uint32_t count, uint8_t temp_int, uint8_t temp_dec,
                   uint8_t hum_int, uint8_t hum_dec, char* buffer);

#endif /* UTILS_H_ */
