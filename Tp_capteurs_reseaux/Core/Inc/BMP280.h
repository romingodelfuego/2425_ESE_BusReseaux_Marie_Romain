/*
 * BMP280.h
 *
 *  Created on: Oct 16, 2024
 *      Author: mariecaronello
 */


#ifndef SRC_BMP280_SIMPLE_H_
#define SRC_BMP280_SIMPLE_H_

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


static const uint8_t BMP280_ADDR = 0x77 << 1; // Use 8-bit address
// static const uint8_t BMP280_ADDR = 0x76 << 1; // Use 8-bit address

static const uint8_t BMP280_ID_REG = 0xD0;
static const uint8_t BMP280_ID_LEN = 1;
static const uint8_t BMP280_ID_VAL = 0x58;

static const uint8_t BMP280_CTRL_MEAS_REG = 0xF4;

static const uint8_t BMP280_PRES_REG_MSB = 0xF7;
static const uint8_t BMP280_PRES_LEN = 3;

static const uint8_t BMP280_TEMP_REG_MSB = 0xFA;
static const uint8_t BMP280_TEMP_LEN = 3;

static const uint8_t BMP280_TRIM_REG_MSB = 0x88;
static const uint8_t BMP280_TRIM_LEN = 12 * 2;

//extern int temp_calib;  // Déclaration externe
//extern int pres_calib;

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
