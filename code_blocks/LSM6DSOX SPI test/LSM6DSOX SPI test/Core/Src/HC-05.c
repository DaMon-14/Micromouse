/*
 * HC-05.c
 *
 *  Created on: Aug 24, 2026
 *      Author: david
 */

#include "main.h"
#include "HC-05.h"
#include <string.h>

#define RX_BUFFER_SIZE 50

extern UART_HandleTypeDef huart1; // HC-05 Bluetooth UART
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_byte;
static uint8_t rx_index = 0;
static uint8_t command_ready = 0;

/**
 * @param msg Poninter to the message
 * @return If transmit OK or not
 */
uint8_t BLE_Write(uint8_t *msg) {
	if (!BLE_IsConnected())
		return 0;
	HAL_UART_Transmit(&huart1, msg, strlen((char*) msg), 50);
	return 1;
}

/**
 *
 * @param echo If the data should be echoed back or not
 * @return Recieved message pointer
 */
uint8_t* BLE_Read_Msg(uint8_t echo) {
	if (command_ready == 1) {
		command_ready = 0;
		if (echo == 1) {
			BLE_Write(rx_buffer);
		}
		return rx_buffer;
	}
	return 0;
}

/**
 * @return If a device is connected or not
 */
uint8_t BLE_IsConnected(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET) {
		return 1;
	}
	return 0;
}

/**
 * Arms the interrupt
 */
void BLE_Init(void) {
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

/**
 * @param huart Which UART triggered a error
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		// If the UART crashes due to noise or overrun, instantly re-arm the interrupt
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	}
}

/**
 * @param huart Which UART triggered the interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //Automatically called by the interrupt
{
	if (huart->Instance == USART1) {
		// 1. Check if the incoming byte is an end-of-message character
		if (rx_byte == '\n' || rx_byte == '\r') {
			if (rx_index > 0) {
				rx_buffer[rx_index] = '\r'; // Null-terminate the string strictly
				rx_buffer[rx_index + 1] = '\n';
				command_ready = 1;  // Flag the main loop to process the command
				for (uint8_t i = rx_index + 2; i < RX_BUFFER_SIZE - 1; i++) {
					rx_buffer[i] = 0;
				}
				rx_index = 0;            // Reset the index for the next message

			}
		}
		// 2. Otherwise, keep saving the standard characters
		else {
			if (rx_index < RX_BUFFER_SIZE - 3) {
				rx_buffer[rx_index] = rx_byte;
				rx_index++;
			}
		}

		// 3. Re-arm the interrupt to catch the next incoming byte
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	}
}
