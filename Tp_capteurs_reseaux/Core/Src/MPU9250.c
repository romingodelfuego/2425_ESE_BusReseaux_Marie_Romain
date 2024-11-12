/*
 * MPU9250.c
 *
 *  Created on: Nov 11, 2024
 *      Author: mariecaronello
 */

#include "stdio.h"
#include "stdlib.h"

#include "main.h"
#include "MPU9250.h"
#include "define.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t MPU9250_ADDR = 0x68 << 1;  	// Adresse I2C décalée de 1 bit
uint8_t MPU9250_ID_REG = 0x75;  	// Registre d'identification
uint8_t MPU9250_PWR_MGMT = 0x6B;  	// Registre de gestion d'alimentation
uint8_t MPU9250_ACCEL_H = 0x3B;  	// Premier registre pour les données d'accélération
uint8_t MPU9250_GYRO_H = 0x43;  	// Premier registre pour les données de gyroscope
uint8_t MPU9250_MAG_L = 0x03;  		// Premier registre pour les données du magnétomètre (AK8963)
uint8_t MPU9250_ID = 0x71;  		// Identifiant du MPU9250


int MPU9250_check() {
	printf("\r\nChecking for MPU9250...\r\n");

    uint8_t data = 0;

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, &MPU9250_ID_REG, 1, HAL_MAX_DELAY) != HAL_OK) {
        printf("Problem with check (I2C Transmit)\r\n");
        return 1;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, MPU9250_ADDR, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
        printf("Problem with check (I2C Receive) \r\n");
        return 1;
    }

    if (data == MPU9250_ID) {
        printf("MPU9250 Ok\r\n");
        return 0;
    }
    else {
        printf("MPU9250 not Ok\r\n");
        return 1;
    }
}

int MPU9250_init() {
	// valeur pour réveiller le capteur
    uint8_t init_data = 0x00;

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, &MPU9250_PWR_MGMT, 1, HAL_MAX_DELAY) != HAL_OK ||
        HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, &init_data, 1, HAL_MAX_DELAY) != HAL_OK) {
        printf("Config error MPU9250\n");
    }
    else{
    	printf("Config Ok");
    	return 1;
    }

    HAL_Delay(100);  // Attendre que le capteur se stabilise

    return 0;
}

static int MPU9250_read_data(uint8_t reg, SensorData *data) {
    uint8_t buf[6];

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU9250_ADDR, &reg, 1, HAL_MAX_DELAY) != HAL_OK){
    	printf("xErreur with I2C Transmit\r\n");
    	return 1;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, MPU9250_ADDR, buf, 6, HAL_MAX_DELAY) != HAL_OK) {
        printf("xErreur with I2C Receive\r\n");
        return 1;
    }

    // Combiner les deux octets pour obtenir les valeurs 16 bits
    data->x = (int16_t)(buf[0] << 8 | buf[1]);
    data->y = (int16_t)(buf[2] << 8 | buf[3]);
    data->z = (int16_t)(buf[4] << 8 | buf[5]);

    return 0;
}

int MPU9250_read_accel(SensorData *accel) {
    return MPU9250_read_data(MPU9250_ACCEL_H, accel);
}

int MPU9250_read_gyro(SensorData *gyro) {
    return MPU9250_read_data(MPU9250_GYRO_H, gyro);
}

int MPU9250_read_mag(SensorData *mag) {
    uint8_t mag_addr = 0x0C << 1;

    if (HAL_I2C_Master_Transmit(&hi2c1, mag_addr, &MPU9250_MAG_L, 1, HAL_MAX_DELAY) != HAL_OK){
    	printf("Erreur de lecture du magnétomètre AK8963\n");
    	return 1;
    }
    if(HAL_I2C_Master_Receive(&hi2c1, mag_addr, (uint8_t *)mag, 6, HAL_MAX_DELAY) != HAL_OK) {
        printf("Erreur de lecture du magnétomètre AK8963\n");
        return 1;
    }

    mag->x = (int16_t)((mag->x & 0xFF00) >> 8 | (mag->x & 0x00FF) << 8);
    mag->y = (int16_t)((mag->y & 0xFF00) >> 8 | (mag->y & 0x00FF) << 8);
    mag->z = (int16_t)((mag->z & 0xFF00) >> 8 | (mag->z & 0x00FF) << 8);

    return 0;
}

