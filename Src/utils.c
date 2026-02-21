/*
 * utils.c
 *
 *  Created on: Feb 20, 2026
 *      Author: Rubin Khadka
 */

#include "utils.h"

void itoa_16(int16_t value, char *buffer)
{
  char *ptr = buffer;

  // Handle negative numbers
  if(value < 0)
  {
    *ptr++ = '-';
    value = -value;
  }

  // Extract digits in reverse order
  char temp[6];
  uint8_t i = 0;
  do
  {
    temp[i++] = (value % 10) + '0';
    value /= 10;
  }
  while(value > 0);

  // Reverse digits into buffer
  while(i-- > 0)
  {
    *ptr++ = temp[i];
  }
  *ptr = '\0';
}

void itoa_8(uint8_t value, char *buffer)
{
  char *ptr = buffer;
  char temp[3];
  uint8_t i = 0;

  // Special case for zero
  if(value == 0)
  {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
  }

  // Extract digits
  do
  {
    temp[i++] = (value % 10) + '0';
    value /= 10;
  }
  while(value > 0);

  // Reverse digits
  while(i-- > 0)
  {
    *ptr++ = temp[i];
  }
  *ptr = '\0';
}

void format_value(uint8_t integer, uint8_t decimal, char *buffer, char unit)
{
  char *ptr = buffer;

  // Integer part (1-2 digits)
  if(integer >= 10)
  {
    *ptr++ = '0' + (integer / 10);
  }
  *ptr++ = '0' + (integer % 10);

  // Decimal point and tenths
  *ptr++ = '.';
  *ptr++ = '0' + decimal;

  // Unit
  *ptr++ = unit;
  *ptr = '\0';
}

void format_reading(uint32_t count, uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec, char *buffer)
{
  char *ptr = buffer;
  char temp[8];
  uint8_t i;

  // Add '['
  *ptr++ = '[';

  // Convert count to string (up to 5 digits)
  if(count >= 10000)
    *ptr++ = '0' + (count / 10000) % 10;
  if(count >= 1000)
    *ptr++ = '0' + (count / 1000) % 10;
  if(count >= 100)
    *ptr++ = '0' + (count / 100) % 10;
  if(count >= 10)
    *ptr++ = '0' + (count / 10) % 10;
  *ptr++ = '0' + (count % 10);

  // Add '] '
  *ptr++ = ']';
  *ptr++ = ' ';

  // Add "Temp: "
  *ptr++ = 'T';
  *ptr++ = 'e';
  *ptr++ = 'm';
  *ptr++ = 'p';
  *ptr++ = ':';
  *ptr++ = ' ';

  // Format temperature
  format_value(temp_int, temp_dec, temp, 'C');
  i = 0;
  while(temp[i])
  {
    *ptr++ = temp[i++];
  }

  // Add ", Hum: "
  *ptr++ = ',';
  *ptr++ = ' ';
  *ptr++ = 'H';
  *ptr++ = 'u';
  *ptr++ = 'm';
  *ptr++ = ':';
  *ptr++ = ' ';

  // Format humidity
  format_value(hum_int, hum_dec, temp, '%');
  i = 0;
  while(temp[i])
  {
    *ptr++ = temp[i++];
  }

  // Add newline
  *ptr++ = '\r';
  *ptr++ = '\n';
  *ptr = '\0';
}
