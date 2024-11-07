/*
 * shell.h
 *
 *  Created on: Nov 7, 2024
 *      Author: romingo
 */

#ifndef INC_SHELL_H_
#define INC_SHELL_H_

#include "define.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "BMP280.h"

void shellv1();
void shellv2();

void subfunct_GET_T(char* value);
void subfunct_GET_P(char* value);
void subfunct_SET_K(char* value);


typedef struct{
	char* requete;
	void (*funct)(char*);
}MAPPER;


#endif /* INC_SHELL_H_ */
