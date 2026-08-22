/*
 * LSM6DSOX.c
 *
 *  Created on: Aug 22, 2026
 *      Author: david
 */
#include "main.h"
#include "LSM6DSOX.h"

extern SPI_HandleTypeDef hspi2;

uint8_t Read_WhoAmI(void) {
	uint8_t spi_tx[2];
	uint8_t spi_rx[2];

	// 1. Prepare the SPI buffer
	// 0x0F is the WHO_AM_I register.
	// We bitwise OR it with 0x80 to set the MSB to 1 (Read command).
	spi_tx[0] = 0x0F | 0x80;
	spi_tx[1] = 0x00; // Dummy byte to clock in the sensor's response

	// 2. Pull the CS pin (PB12) LOW to wake up the sensor
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	// 3. Transmit the register address and receive the response simultaneously
	HAL_SPI_TransmitReceive(&hspi2, spi_tx, spi_rx, 2, 10);

	// 4. Pull the CS pin (PB12) HIGH to release the sensor
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	return spi_rx[1];
}

uint8_t Read_Reg(uint8_t reg) {
	if ((reg & 0x80) == 0x80) {
		return 0;
	}

	uint8_t spi_tx[2];
	uint8_t spi_rx[2];

	// 1. Prepare the SPI buffer
	// 0x0F is the WHO_AM_I register.
	// We bitwise OR it with 0x80 to set the MSB to 1 (Read command).
	spi_tx[0] = reg | 0x80;
	spi_tx[1] = 0x00; // Dummy byte to clock in the sensor's response

	// 2. Pull the CS pin (PB12) LOW to wake up the sensor
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	// 3. Transmit the register address and receive the response simultaneously
	HAL_SPI_TransmitReceive(&hspi2, spi_tx, spi_rx, 2, 10);

	// 4. Pull the CS pin (PB12) HIGH to release the sensor
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	return spi_rx[1];
}

void Write_Reg(uint8_t addr, uint8_t data) {
	if ((addr & 0x80) == 0x80) {
		return;
	}

	uint8_t spi_tx[2];
	uint8_t spi_rx[2];

	spi_tx[0] = addr;
	spi_tx[1] = data;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, spi_tx, spi_rx, 2, 10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

void IMU_Init(void) {
	Write_Reg(0x10, 0x40);
	Write_Reg(0x11, 0x40);
}
