/*
 * BMP280.h
 *
 *  Created on: Oct 16, 2024
 *      Author: mariecaronello
 */


#ifndef SRC_BMP280_SIMPLE_H_
#define SRC_BMP280_SIMPLE_H_

/* Structures */

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;

typedef struct{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
}TEMP_COEFF;

typedef struct{
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
}PRES_COEFF;


int BMP280_check();
int BMP280_init();
BMP280_S32_t BMP280_get_temperature();
BMP280_S32_t BMP280_get_pressure();
void BMP280_Read_Reg(uint8_t * buf,uint8_t reg, uint8_t value);
int BMP280_Write_Reg(uint8_t reg, uint8_t value);

TEMP_COEFF get_coef_temperature();
PRES_COEFF get_coef_pressure();

BMP280_S32_t compensate_temperature();
BMP280_U32_t compensate_pressure();

#endif /* SRC_BMP280_H_ */
