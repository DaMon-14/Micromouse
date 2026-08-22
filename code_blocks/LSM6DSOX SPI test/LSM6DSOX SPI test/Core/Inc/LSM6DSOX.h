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

#endif /* INC_LSM6DSOX_H_ */
