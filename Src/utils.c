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

void format_reading(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec, char *buffer)
{
  char *ptr = buffer;
  char temp[8];
  uint8_t i;

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

void format_temp_hum(char *buffer, uint8_t temp, uint8_t hum)
{
  char *ptr = buffer;

  // "T:24C H:45%"
  *ptr++ = 'T';
  *ptr++ = ':';

  // Temperature (1-2 digits)
  if(temp >= 10)
    *ptr++ = '0' + (temp / 10);
  *ptr++ = '0' + (temp % 10);

  *ptr++ = 'C';
  *ptr++ = ' ';
  *ptr++ = 'H';
  *ptr++ = ':';

  // Humidity (1-2 digits)
  if(hum >= 10)
    *ptr++ = '0' + (hum / 10);
  *ptr++ = '0' + (hum % 10);

  *ptr++ = '%';
  *ptr++ = '\r';
  *ptr++ = '\n';
  *ptr = '\0';
}

void format_accel(char *buffer, int16_t ax, int16_t ay, int16_t az)
{
  char *ptr = buffer;
  char num[8];

  // "AX:123 AY:456 AZ:789"

  // AX
  *ptr++ = 'A';
  *ptr++ = 'X';
  *ptr++ = ':';
  itoa_16(ax, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = ' ';

  // AY
  *ptr++ = 'A';
  *ptr++ = 'Y';
  *ptr++ = ':';
  itoa_16(ay, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = ' ';

  // AZ
  *ptr++ = 'A';
  *ptr++ = 'Z';
  *ptr++ = ':';
  itoa_16(az, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = '\r';
  *ptr++ = '\n';
  *ptr = '\0';
}

void format_gyro(char *buffer, int16_t gx, int16_t gy, int16_t gz)
{
  char *ptr = buffer;
  char num[8];

  // "GX:123 GY:456 GZ:789"

  // GX
  *ptr++ = 'G';
  *ptr++ = 'X';
  *ptr++ = ':';
  itoa_16(gx, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = ' ';

  // GY
  *ptr++ = 'G';
  *ptr++ = 'Y';
  *ptr++ = ':';
  itoa_16(gy, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = ' ';

  // GZ
  *ptr++ = 'G';
  *ptr++ = 'Z';
  *ptr++ = ':';
  itoa_16(gz, num);
  for(char *s = num; *s; s++)
    *ptr++ = *s;

  *ptr++ = '\r';
  *ptr++ = '\n';
  *ptr = '\0';
}
