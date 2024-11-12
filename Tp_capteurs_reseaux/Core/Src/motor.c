/*
 *	motor.c
 *
 *  Created on: Nov 7, 2024
 *      Author: romingo
 */

#include "motor.h"


extern CAN_HandleTypeDef hcan1;

void motor_Angle(ANGLE_SENS sens,uint8_t angle){
	CAN_TxHeaderTypeDef pHeader =(CAN_TxHeaderTypeDef){
		.StdId = 0x61,
		.ExtId = 0x01,
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.DLC = 0x02,
		.TransmitGlobalTime = DISABLE
	};
	uint32_t pTxMailbox;
	uint8_t aData[2]={sens,angle};
	HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox);
}


//FONCTION POUR MODIFIER L'ANGLE DU MOTEUR
/*
 *
for (int i = 0;i<100;i++){
	uint8_t sens = i%2;
	uint8_t aData[2]= {0x55,sens};
	printf("Avant envoie CAN ,nb: %lu\r\n",HAL_CAN_IsTxMessagePending(&hcan1, pTxMailbox));
	if (HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox)!= HAL_OK){printf("Erreur Sending\r\n");}
	printf("Apres envoie CAN ,nb: %lu\r\n",HAL_CAN_IsTxMessagePending(&hcan1, pTxMailbox));
	HAL_Delay(2000);

}
 */
