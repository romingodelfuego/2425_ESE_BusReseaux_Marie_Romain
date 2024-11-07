/*
 * motor.h
 *
 *  Created on: Nov 7, 2024
 *      Author: romingo
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "define.h"
#include "stdio.h"
#include "can.h"
#include "main.h"

typedef enum{
	CLOCKWISE = 0x00,
	ANTI_CLOCKWISE = 0x01,
}ANGLE_SENS;


void motor_Angle(ANGLE_SENS sens,uint8_t angle);
#endif /* INC_MOTOR_H_ */
