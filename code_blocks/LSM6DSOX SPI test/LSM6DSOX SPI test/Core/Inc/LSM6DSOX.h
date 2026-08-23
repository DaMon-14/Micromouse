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

int16_t* Read_IMU_Data_Raw(void);
float* Read_IMU_Data(void);

void IMU_Calibrate(void);
float* Read_IMU_Data_Calibrated(void);
#endif /* INC_LSM6DSOX_H_ */
