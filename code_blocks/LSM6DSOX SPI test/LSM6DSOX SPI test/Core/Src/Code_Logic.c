/*
 * Code_Logic.c
 *
 *  Created on: Aug 22, 2026
 *      Author: david
 */
#include "main.h"
#include "Code_Logic.h"


#include <stdio.h>
#include <string.h>

#include "LSM6DSOX.h"


extern UART_HandleTypeDef huart2;



uint32_t previous_spi_timestamp = 0;

void User_Code_Init(void) {

}

void User_Code_Run(void) {
	// Get the current time in milliseconds[cite: 1]
	uint32_t current_timestamp = HAL_GetTick();

	// Check if 500 milliseconds have passed to read the sensor
	if (current_timestamp - previous_spi_timestamp >= 500) {
		previous_spi_timestamp = current_timestamp;
		uint8_t ret = Read_WhoAmI();

		// 5. Create an empty character array (buffer) large enough to hold the message[cite: 1]
		char msg[50];

		// 6. Format the string to display the WHO_AM_I result (expected: 0x6C)
		sprintf(msg, "[%lu ms] LSM6DSOX WHO_AM_I: 0x%02X\r\n",
				current_timestamp, ret);

		// 7. Transmit the message over UART2[cite: 1]
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), 10);
	}
}

void User_Code_End(void) {

}
