/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Rubin Khadka
 * @brief          : DHT11 MP6050 Sensors with LCD and UART Display
 ******************************************************************************
 */

#include "stm32f103xb.h"
#include "timer1.h"
#include "timer2.h"
#include "dht11.h"
#include "uart.h"
#include "utils.h"

#define MAX_RETRIES 5

// Buffer for UART output
char output_buff[64];

int main(void) {
	// Variables for sensor data
	uint8_t hum_byte1, hum_byte2, temp_byte1, temp_byte2, checksum;
	uint32_t read_count = 0;

	// Initialize ALL modules
	TIMER1_Init();     // 1µs resolution for DHT11
	TIMER2_Init();     // 1ms resolution for long delays
	USART1_Init();     // UART for output
	DHT11_Init();      // DHT11 sensor on PB0

	// === Welcome message ===
	USART1_SendString("\r\n========================================\r\n");
	USART1_SendString("DHT11 SENSOR READER \r\n");
	USART1_SendString("========================================\r\n");

	// Give sensor time to power up
	TIMER2_Delay_ms(1000);

	// Main Loop
	while (1) {
		read_count++;
		int success = 0;

		// Disable Interrupts for the entire DHT11 transaction
		uint32_t primask = __get_PRIMASK();
		__disable_irq();

		// Try up to MAX_RETRIES times
		for (int retry = 0; retry < MAX_RETRIES; retry++) {
			DHT11_Start();

			if (DHT11_Check_Response()) {
				hum_byte1 = DHT11_Read();
				hum_byte2 = DHT11_Read();
				temp_byte1 = DHT11_Read();
				temp_byte2 = DHT11_Read();
				checksum = DHT11_Read();

				uint8_t calc_checksum = hum_byte1 + hum_byte2 + temp_byte1
						+ temp_byte2;

				if (calc_checksum == checksum) {
					success = 1;
					break;  // Success! Exit retry loop
				}
			}

			// Small delay before retry
			TIMER1_Delay_ms(100);
		}

		// Re-enable interrupts after DHT11 communication
		__set_PRIMASK(primask);

		// Process results with interrupts enabled
		if (success) {
			format_reading(read_count, temp_byte1, temp_byte2, hum_byte1,
					hum_byte2, output_buff);
			USART1_SendString(output_buff);
		} else {
			USART1_SendString("No response after retries\r\n");
		}

		TIMER2_Delay_ms(2000);
	}
}
