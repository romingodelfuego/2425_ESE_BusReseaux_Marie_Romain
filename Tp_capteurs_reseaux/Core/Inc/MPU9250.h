/*
 * MPU9250.h
 *
 *  Created on: Nov 11, 2024
 *      Author: mariecaronello
 */

#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_

// Structures pour stocker les données du capteur
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} SensorData;

typedef struct {
    SensorData accel;
    SensorData gyro;
    SensorData mag;
} MPU9250_Data;

// Prototypes des fonctions
int MPU9250_init();
int MPU9250_read_accel(SensorData *accel);
int MPU9250_read_gyro(SensorData *gyro);
int MPU9250_read_mag(SensorData *mag);
int MPU9250_who_am_i();
void MPU9250_send_data(MPU9250_Data *data);



#endif /* INC_MPU9250_H_ */
