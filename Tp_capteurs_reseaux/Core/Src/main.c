/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_CAN2_Init();
	/* USER CODE BEGIN 2 */
	uint8_t cmd = 0xD0;
	uint8_t SlaveResponse;

	printf("\r\n=== START I2C ===\r\n");
	if (HAL_I2C_Master_Transmit(
			&hi2c1,
			(uint16_t)(0x77<<1),
			&cmd,
			1,
			1000)
			!= HAL_OK){ printf("xErreur Transmit\r\n");
	}

	if (HAL_I2C_Master_Receive(
			&hi2c1,
			(uint16_t)(0x77<<1),
			&SlaveResponse,
			1,
			1000)
			!= HAL_OK){ printf("xErreur Receive\r\n");
	}
	printf(">ID Slave: 0x%02X\r\n",SlaveResponse);
	/*
	 * 		CONFIGURATION DU CONTROLE MESURE
	 */
	uint8_t reg_ctrlMes = 0xF4;
	uint8_t config_ctrlMes = 0b01010111; // 010: temperature, 101: press, 11: mode normal
	uint8_t mess_ctrlMes[2] = {reg_ctrlMes,config_ctrlMes};
	if (HAL_I2C_Master_Transmit(
			&hi2c1,
			(uint16_t)(0x77<<1),
			mess_ctrlMes,
			2,
			1000)
			!= HAL_OK){ printf("xErreur Transmit\r\n");
	}

	if (HAL_I2C_Master_Receive(
			&hi2c1,
			(uint16_t)(0x77<<1),
			&SlaveResponse,
			1,
			1000)
			!= HAL_OK){ printf("xErreur Receive\r\n");
	}
	printf(">Config Control: 0x%02X\r\n",SlaveResponse);

	/*
	 * 		------- RÉCUPÉRATION ÉTALONNAGE, TEMPÉRATURE ET PRESSION -------
	 */
	uint8_t coeff_TEMP[3*2];
	uint8_t reg_trimming_TEMP=0x88;
	HAL_I2C_Master_Transmit(
			&hi2c1,(uint16_t)(0x77<<1),
			&reg_trimming_TEMP,
			1,1000);
	HAL_I2C_Master_Receive(
			&hi2c1,(uint16_t)(0x77<<1),
			coeff_TEMP,
			3*2, 1000); // Tout est sotcké sur 16 bits donc on regarde 2 fois 8 bits

	for (int i = 0; i < 3; i++){ // For temperature coeff
		uint16_t coeff = coeff_TEMP[i]+coeff_TEMP[i+1];
		printf("Coeff %i : %u\r\n",i,coeff);
		i++; // Pour regarder les valeurs 2 a 2
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		uint8_t reg_TEMP = 0xF7;
		uint8_t reg_PRESS = 0xFA;

		uint8_t rep_TEMP[3];
		uint8_t rep_PRESS[3];
		HAL_I2C_Master_Transmit(&hi2c1,(uint16_t)(0x77<<1),
				&reg_TEMP,
				1,1000);

		HAL_I2C_Master_Receive(&hi2c1,(uint16_t)(0x77<<1),
				&rep_TEMP,
				3, 1000); // TEMP ecrit sur 20 bits

		HAL_I2C_Master_Transmit(&hi2c1,(uint16_t)(0x77<<1),
				&reg_PRESS,
				1,1000);

		HAL_I2C_Master_Receive(&hi2c1,(uint16_t)(0x77<<1),
				&rep_PRESS,
				3, 1000); // PRESS écrit sur 20 bits

		uint32_t PRESS=0;
		for (int i = 0; i<3;i++){
			PRESS += rep_PRESS[i]>>(8*i);
		}

		/* CALCUL DE LA TEMP REEL */
		uint32_t TEMP=0;
		for (int i = 0; i<3;i++){
			TEMP += rep_TEMP[i]>>(8*i);
		}


		printf("TEMP: %u\r\nPRESS: %u\r\n",rep_TEMP,rep_PRESS);
		HAL_Delay(500);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
