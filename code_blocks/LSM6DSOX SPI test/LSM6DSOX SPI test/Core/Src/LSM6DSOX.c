/*
 * LSM6DSOX.c
 *
 *  Created on: Aug 22, 2026
 *      Author: david
 */
#include "main.h"
#include "LSM6DSOX.h"

extern SPI_HandleTypeDef hspi2;

static int16_t raw_data[6];
static float data[6];
static float calibrated_data[6];
static float bias[6];

float* Read_IMU_Data_Calibrated(void){
	Read_IMU_Data();
	calibrated_data[0]=data[0]-bias[0];
	calibrated_data[1]=data[1]-bias[1];
	calibrated_data[2]=data[2]-bias[2];
	calibrated_data[3]=data[3]-bias[3];
	calibrated_data[4]=data[4]-bias[4];
	calibrated_data[5]=data[5]-bias[5];

	return calibrated_data;
}

void IMU_Calibrate(void) {
	int samples = 250;
	float temp_gyro[3] = { 0.0f, 0.0f, 0.0f };
	float temp_accel[3] = { 0.0f, 0.0f, 0.0f };

	// Keep the board still during this loop!
	for (int i = 0; i < samples; i++) {
		float *motion = Read_IMU_Data();
		temp_gyro[0] += motion[0]; // Gyro X
		temp_gyro[1] += motion[1]; // Gyro Y
		temp_gyro[2] += motion[2]; // Gyro Z

		temp_accel[0] += motion[3];
		temp_accel[1] += motion[4];
		temp_accel[2] += motion[5];

		HAL_Delay(1);
	}

	// Average out the resting offset
	bias[0] = temp_gyro[0] / samples;
	bias[1] = temp_gyro[1] / samples;
	bias[2] = temp_gyro[2] / samples;
	bias[3] = temp_accel[0] / samples;
	bias[4] = temp_accel[1] / samples;
	bias[5] = temp_accel[2] / samples;
}

float* Read_IMU_Data(void) {
	Read_IMU_Data_Raw();
	//gyro
	data[0] = (raw_data[0] * 8.75f) / 1000.0f;
	data[1] = (raw_data[1] * 8.75f) / 1000.0f;
	data[2] = (raw_data[2] * 8.75f) / 1000.0f;
	//accelerometer
	data[3] = (raw_data[3] * 0.061f) / 1000.0f;
	data[4] = (raw_data[4] * 0.061f) / 1000.0f;
	data[5] = (raw_data[5] * 0.061f) / 1000.0f;

	return data;
}

int16_t* Read_IMU_Data_Raw(void) {
	uint8_t spi_tx[13] = { 0 };
	uint8_t spi_rx[13] = { 0 };

	// Burst read starting from OUTX_L_G (0x22) with Read bit set (0x80)
	spi_tx[0] = 0x22 | 0x80;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, spi_tx, spi_rx, 13, 20);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	// Bitwise Reassembly (Low byte first, then High byte shifted by 8)
	//gyro
	raw_data[0] = (int16_t) ((spi_rx[2] << 8) | spi_rx[1]);
	raw_data[1] = (int16_t) ((spi_rx[4] << 8) | spi_rx[3]);
	raw_data[2] = (int16_t) ((spi_rx[6] << 8) | spi_rx[5]);

	//accelerometer
	raw_data[3] = (int16_t) ((spi_rx[8] << 8) | spi_rx[7]);
	raw_data[4] = (int16_t) ((spi_rx[10] << 8) | spi_rx[9]);
	raw_data[5] = (int16_t) ((spi_rx[12] << 8) | spi_rx[11]);

	return raw_data;
}

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
