/*
 * ADXL345.h
 *
 *  Created on: Nov 11, 2024
 *      Author: mariecaronello
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

int  ADXL345_check();
int ADXL345_init();
int ADXL345_read_axes(int16_t* x, int16_t* y, int16_t* z);

#endif /* INC_ADXL345_H_ */
