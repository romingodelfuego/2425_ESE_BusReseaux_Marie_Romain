/*
 * ADXL345.c
 *
 *  Created on: Nov 11, 2024
 *      Author: mariecaronello
 */


#include "stdio.h"
#include "stdlib.h"

#include "main.h"
#include "ADXL345.h"
#include "define.h"

uint8_t ADXL345_DEVID_REG = 0x00;  // ID du périphérique
uint8_t ADXL345_DEVID_VAL = 0xE5;
uint8_t ADXL345_POWER_CTL = 0x2D;  // Contrôle de l'alimentation
uint8_t ADXL345_DATA_FORMAT = 0x31;  // Format des données
uint8_t ADXL345_DATAX0 = 0x32;  // LSB de l'axe X
uint8_t ADXL345_DATAX1 = 0x33;  // MSB de l'axe X
uint8_t ADXL345_DATAY0 = 0x34;  // LSB de l'axe Y
uint8_t ADXL345_DATAY1 = 0x35;  // MSB de l'axe Y
uint8_t ADXL345_DATAZ0 = 0x36;  // LSB de l'axe Z
uint8_t ADXL345_DATAZ1 = 0x37;  // MSB de l'axe Z

int  ADXL345_check() {

	uint8_t data;

	/*Vérifier l'identifiant du périphérique
	 *
	 * */
	data = ADXL345_DEVID_REG;
	printf("\r\nChecking for ADXL345...\r\n");
	if (HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		printf("Problem with check (I2C Transmit)\r\n");
		return 1;
	}
	if (HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDR, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		printf("Problem with check (I2C Transmit)\r\n");
		return 1;
	}

	// 0xE5 est l'ID attendu pour ADXL345
	if (data == ADXL345_DEVID_VAL) {
		printf("Ok\r\n");
		return 1;
	}
	else {
		printf("not Ok!\r\n");
		return 1;
	}

}

int ADXL345_init(){
	/*
	 * Mettre l'ADXL345 en mode de mesure
	 * */

	HAL_StatusTypeDef ret;


	/*
	 * Configuration du POWER_CTRL
	 * */

	uint8_t power_ctl_reg = ADXL345_POWER_CTL;
	ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &power_ctl_reg, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		printf("Error while transmitting Power Control register address\r\n");
		return 1;
	}

	data = 0x08; // Mode de mesure
	ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &data, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		printf("Error while configuring Power Control register\r\n");
		return 1;
	}

	printf("Config Ok du Power Control\r\n");

	/*
	 * Configuration de DATA_FORMAT
	 * +/-2g et 10 bits
	 * */

	data = 0x08;
	uint8_t data_format_reg = ADXL345_DATA_FORMAT;

	ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &data_format_reg, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		printf("Error while transmitting Data Format register address\r\n");
		return 1;
	}

	ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &data, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		printf("Error while configuring Data Format register\r\n");
		return 1;
	}

	printf("Config Ok de Data Format\r\n");
	return 0;

}




int ADXL345_read_axes(int16_t* x, int16_t* y, int16_t* z) {


    HAL_StatusTypeDef ret;

    // tableau pour stocker les données 2 donnée par axe
    uint8_t data[2];

    /*
     * Lecture des données de l'axe X
     * */

    uint8_t reg = ADXL345_DATAX0;
    ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &reg, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while transmitting X register address\r\n");
        return 1;
    }

    // Lire 2 octets (valeur de l'axe X)
    ret = HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDR, data, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while reading X data\r\n");
        return 1;
    }
    // Combine les deux octets pour obtenir la valeur de l'axe X
    *x = (int16_t)((data[1] << 8) | data[0]);

    /*
     * Lecture des données de l'axe Y
     * */

    reg = ADXL345_DATAY0;
    ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &reg, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while transmitting Y register address\r\n");
        return 1;
    }

    ret = HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDR, data, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while reading Y data\r\n");
        return 1;
    }
    *y = (int16_t)((data[1] << 8) | data[0]);

    /*
     * Lecture des données de l'axe Z
     * */

    reg = ADXL345_DATAZ0;
    ret = HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDR, &reg, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while transmitting Z register address\r\n");
        return 1;
    }

    ret = HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDR, data, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Error while reading Z data\r\n");
        return 1;
    }
    *z = (int16_t)((data[1] << 8) | data[0]);

    printf("X: %d, Y: %d, Z: %d\r\n", x, y, z);
    return 0;

}
