/*
 * BMP280.c
 *
 *  Created on: Oct 16, 2024
 *      Author: marie caronello
 */


#include "stdio.h"
#include "stdlib.h"

#include "main.h"
#include "BMP280.h"
#include "define.h"

extern I2C_HandleTypeDef hi2c1;

PRES_COEFF  pres_coeff;
TEMP_COEFF temp_coeff;
BMP280_S32_t t_fine;

int BMP280_check() {
	uint8_t buf[1];
	HAL_StatusTypeDef ret;
	buf[0] = BMP280_ID_REG;
	printf("\r\nChecking for BMP280...\r\n");

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 1, HAL_MAX_DELAY);
	if (ret != 0) {
		printf("Problem with check (I2C Transmit)\r\n");
	}

	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, BMP280_ID_LEN,
			HAL_MAX_DELAY);
	if (ret != 0) {
		printf("Problem with check (I2C Receive) \r\n");
	}

	printf("Id: 0x%x...", buf[0]);
	if (buf[0] == BMP280_ID_VAL) {
		printf("Ok\r\n");
		return 0;
	} else {
		printf("not Ok!\r\n");
		return 1;
	}
}


TEMP_COEFF get_coef_temperature(){
	uint8_t coeff_TEMP[3*2];
	uint8_t reg_trimming_TEMP=0x88;
	HAL_I2C_Master_Transmit(&hi2c1,(uint16_t)(BMP280_ADDR), &reg_trimming_TEMP, 1,1000);
	HAL_I2C_Master_Receive(&hi2c1,(uint16_t)(BMP280_ADDR), coeff_TEMP, 3*2, 1000); // Tout est sotcké sur 16 bits donc on regarde 2 fois 8 bits

	TEMP_COEFF temp_coeff=(TEMP_COEFF){
	.dig_T1 = coeff_TEMP[0],
	.dig_T2 = coeff_TEMP[1],
	.dig_T3 = coeff_TEMP[3]
	};
	return temp_coeff;
}

PRES_COEFF get_coef_pressure(){
	uint8_t coeff_PRESS[9*2];
	uint8_t reg_trimming_PRESS=0x8E;
	HAL_I2C_Master_Transmit( &hi2c1,(uint16_t)(BMP280_ADDR), &reg_trimming_PRESS, 1,1000);
	HAL_I2C_Master_Receive( &hi2c1,(uint16_t)(BMP280_ADDR), coeff_PRESS, 9*2, 1000); // Tout est sotcké sur 16 bits donc on regarde 2 fois 8 bits

	pres_coeff=(PRES_COEFF){
	.dig_P1 =coeff_PRESS[0],
	.dig_P2 =coeff_PRESS[1],
	.dig_P3 =coeff_PRESS[2],
	.dig_P4 =coeff_PRESS[3],
	.dig_P5 =coeff_PRESS[4],
	.dig_P6 =coeff_PRESS[5],
	.dig_P7 =coeff_PRESS[6],
	.dig_P8 =coeff_PRESS[7],
	.dig_P9 =coeff_PRESS[8],
	};
	return pres_coeff;
}


int BMP280_init() {
	HAL_StatusTypeDef ret;
	uint8_t ctrl = (0b010 << 5) | (0b101 << 2) | (0b11);
	/* 				osr_t x2       osr_p x16       normal mode   */

	printf("\r\nConfigure BMP280...\r\n");
	ret = BMP280_Write_Reg(BMP280_CTRL_MEAS_REG, ctrl);
	if (ret == 0) {
		printf("Config Ok\r\n");
	} else {
		printf("Config not Ok!\r\n");
		return 1;
	}
	get_coef_pressure();
	get_coef_temperature();
	return 0;
}

int BMP280_Write_Reg(uint8_t reg, uint8_t value) {
	uint8_t buf[3];
	HAL_StatusTypeDef ret;

	buf[0] = reg;
	buf[1] = value;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2, HAL_MAX_DELAY);
	if (ret != 0) {
		printf("xErreur with I2C Transmit\r\n");
	}

	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, 1, HAL_MAX_DELAY);
	if (ret != 0) {
		printf("xErreur with I2C Receive\r\n");
	}

	if (buf[0] == value) {
		return 0;
	} else {
		return 1;
	}
}

void BMP280_Read_Reg(uint8_t *buf, uint8_t reg, uint8_t length) {

	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, HAL_MAX_DELAY);
	if (ret != 0) {
		printf("xErreur with I2C Transmit\r\n");
	}

	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, length,
			HAL_MAX_DELAY);
	if (ret != 0) {
		printf("xErreur with I2C Receive\r\n");
	}
}

BMP280_S32_t BMP280_get_temperature() {
	uint8_t buf[BMP280_TEMP_LEN];
;
	BMP280_S32_t adc_T;

	BMP280_Read_Reg(buf,BMP280_TEMP_REG_MSB, BMP280_TEMP_LEN);

	adc_T = ((BMP280_S32_t) (buf[0]) << 12) | ((BMP280_S32_t) (buf[1]) << 4)
			| ((BMP280_S32_t) (buf[2]) >> 4);

	printf("Temperature: %lu\r\n", adc_T);
	return adc_T;
}

BMP280_S32_t BMP280_get_pressure() {
	uint8_t buf[BMP280_PRES_LEN];

	BMP280_S32_t adc_P;

	BMP280_Read_Reg(buf, BMP280_PRES_REG_MSB, BMP280_PRES_LEN);

	adc_P = ((BMP280_S32_t) (buf[0]) << 12) | ((BMP280_S32_t) (buf[1]) << 4)
			| ((BMP280_S32_t) (buf[2]) >> 4);
	printf("Pressure: %lu\r\n", adc_P);
	return adc_P;
}


BMP280_S32_t compensate_temperature() {

	BMP280_S32_t raw_temp = BMP280_get_temperature();

	BMP280_S32_t var1 = ((((raw_temp >> 3) - (temp_coeff.dig_T1 << 1))) * (temp_coeff.dig_T2)) >> 11;
	BMP280_S32_t var2 = (((((raw_temp >> 4) - (temp_coeff.dig_T1)) * ((raw_temp >> 4) - (temp_coeff.dig_T1))) >> 12) * (temp_coeff.dig_T3)) >> 14;
    t_fine = var1 + var2;
    BMP280_S32_t T = (t_fine * 5 + 128) >> 8;
    return T; // Température compensée

}

BMP280_U32_t compensate_pressure() {
	int raw_press = BMP280_get_pressure();
	BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * pres_coeff.dig_P6;
    var2 = var2 + ((var1 * pres_coeff.dig_P5) << 17);
    var2 = var2 + ((pres_coeff.dig_P4) << 35);
    var1 = ((var1 * var1 * pres_coeff.dig_P3) >> 8) + ((var1 * pres_coeff.dig_P2) << 12);
    var1 = ((((BMP280_S64_t)1) << 47) + var1) * (pres_coeff.dig_P1) >> 33;
    if (var1 == 0) {
        return 0;
    }
    p = 1048576 - raw_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (pres_coeff.dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = (pres_coeff.dig_P8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (pres_coeff.dig_P7 << 4);
    return (BMP280_U32_t)p; // Pression compensée
}

