/*
 * LSM6DSOX.h
 *
 *  Created on: Aug 22, 2026
 *      Author: david
 */

#ifndef INC_LSM6DSOX_H_
#define INC_LSM6DSOX_H_

uint8_t Read_WhoAmI(void);
uint8_t Read_Reg(uint8_t);
void Write_Reg(uint8_t, uint8_t);
void IMU_Init(void);

typedef struct {
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;
} IMU_Data;

IMU_Data Read_IMU_Data_Raw(void);
IMU_Data Read_IMU_Data(void);
#endif /* INC_LSM6DSOX_H_ */
