/*
 * shell.c
 *
 *  Created on: Nov 7, 2024
 *      Author: romingo
 */

#include "shell.h"
#define SIZE_BUFFER 100
extern UART_HandleTypeDef huart1;

uint8_t Rx_index = 0;
char Rx_Buffer[SIZE_BUFFER];

MAPPER mapping[] = {
		{"GET_T\r",subfunct_GET_T},
		{"GET_P\r",subfunct_GET_P},
		{"SET_K", subfunct_SET_K}  // strtok ne permet pas de comparer avec \r
};

void shellv1(){
	uint8_t Rx_data;
	HAL_UART_Receive(&huart1,  (uint8_t*)&Rx_data, 1, 1000);
	Rx_Buffer[Rx_index++] = Rx_data;
	printf("Receive %d\r\n", Rx_data);
	printf("Buffer: %s\r\n", Rx_Buffer);

	if (Rx_data == '\r' || Rx_data == '\n')
	{
		if (strcmp(Rx_Buffer, "get_t\r") == 0 || strcmp(Rx_Buffer, "get_t\n") == 0)
		{
			int32_t get_t;
			get_t = BMP280_get_temperature();
			char value[20];
			int size = sprintf(value, "\r\n%ld\r\n", get_t);
			HAL_UART_Transmit(&huart1, (uint8_t*)value, size, HAL_MAX_DELAY);
		}

		if (strcmp(Rx_Buffer, "get_p\r") == 0 || strcmp(Rx_Buffer, "get_p\n") == 0)
		{
			int32_t get_p;
			get_p = BMP280_get_pressure();
			char value[20];
			int size = sprintf(value, "\r\n%ld\r\n", get_p);
			HAL_UART_Transmit(&huart1, (uint8_t*)value, size - 1, HAL_MAX_DELAY);
		}
		Rx_index = 0;
		memset(Rx_Buffer, 0, SIZE_BUFFER);
	}

	if (Rx_index >= sizeof(Rx_Buffer) - 1)
	{
		Rx_index = 0;
		memset(Rx_Buffer, 0, SIZE_BUFFER);
	}
}

void shellv2(){ //!\\ Pose probleme pour le set_k=1234
	//Recuperons le caractère
	uint8_t Rx_data;
	int isFind;
	HAL_UART_Receive(&huart1,  (uint8_t*)&Rx_data, 1, 1000);
	Rx_Buffer[Rx_index++] = Rx_data;

	// Interaction lors de la touche entrée
	if (Rx_data == '\r'){
		char * command = strtok(Rx_Buffer,"=");// Divise la chaîne à chaque égal, et altere la cahien d'origine
		char* value = strtok(NULL, " ");  // strtok(NULL) reprend là où la première division a été faite
		for (int i=0; i< sizeof(mapping);i++){
			if (mapping[i].requete==command){
				mapping[i].funct(value);
				isFind = 1;
			}
			if (!isFind) {printf("Unknown command : %s\r\n",Rx_Buffer);}
		}
	}
	//Interaction shell normal
	if (Rx_data == '\b'){Rx_Buffer[Rx_index--]='\0';}
	printf("%s",Rx_Buffer);

}
void subfunct_GET_T(char *value){
	int32_t temp;
	temp = BMP280_get_temperature();
	HAL_UART_Transmit(&huart1, (uint8_t*)temp, sizeof(temp), HAL_MAX_DELAY);
	printf("Temperature:%lu°C\r\n",temp);
}
void subfunct_GET_P(char *value){
	int32_t press;
	press = BMP280_get_pressure();
	HAL_UART_Transmit(&huart1, (uint8_t*)press, sizeof(press), HAL_MAX_DELAY);
	printf("Pression:%lu\r\n",press);
}
void subfunct_SET_K(char *value){
	/*Fonction pour modifier le K :
	 * ici
	 */
	HAL_UART_Transmit(&huart1, (uint8_t*)&"SET_K=OK", 10, HAL_MAX_DELAY);
	printf("SET_K=OK");
}
