/*
 * HC-05.h
 *
 *  Created on: Aug 24, 2026
 *      Author: david
 */

#ifndef INC_HC_05_H_
#define INC_HC_05_H_


void BLE_Init(void);
uint8_t BLE_IsConnected(void);
uint8_t* BLE_Read_Msg(uint8_t);
uint8_t BLE_Write(uint8_t *msg);


#endif /* INC_HC_05_H_ */
