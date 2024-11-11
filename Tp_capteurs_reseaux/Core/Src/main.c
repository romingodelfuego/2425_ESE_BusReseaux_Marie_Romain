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
#include "string.h"
#include "BMP280.h"
#include "motor.h"
#include "define.h"
#include "shell.h"
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
	MX_USART1_UART_Init();
	MX_CAN1_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */
#ifdef CAN
	printf("START\r\n");
	if(HAL_CAN_Start(&hcan1)==HAL_ERROR){printf("Erreur initialisation\r\n");}


#endif
#ifdef BMP280
	printf("------BMP280------\r\n");
	BMP280_check();
	BMP280_init();
	uint32_t temp_ref =  BMP280_get_temperature();
#endif

#ifdef ADXL345
	printf("------BMP280------\r\n");
	ADXL345_check();
	ADXL345_init();
#endif

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
#ifdef MOTOR
		uint32_t temp =  BMP280_get_temperature();
		uint8_t angle = (uint8_t)((temp_ref-temp)/2);

		if (angle >180){
			motor_Angle(CLOCKWISE,90);
		}
		else{
			motor_Angle(ANTI_CLOCKWISE,90);

		}
		printf("angle  : %u°\r\n", angle);
		printf("moteur actualisé\r\n");
		HAL_Delay(1000);
		temp_ref = temp;
#endif
#ifdef SHELLV1
		shellv1();
#endif
#ifdef SHELLV2
		shellv2();
#endif


#ifdef ADXL345
	/*
	 * SEND COORDINATES X, Y, Z
	 * */

	int16_t x, y, z;
	char message[50];
	if (ADXL345_read_axes(&x, &y, &z) == 0) {
		snprintf(message, sizeof(message), "X: %d, Y: %d, Z: %d\r\n", x, y, z);
		HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
	}
	else {
		char error_message[] = "Erreur lors de la lecture des axes\r\n";
		HAL_UART_Transmit(&huart1, (uint8_t*)error_message, strlen(error_message), HAL_MAX_DELAY);
	}
#endif



		/*
		 * COMMUNICATION AVEC RASBERRY PI
		 */


		/*
		 * RECEIVE AVEC RASBERRY PI
		 */


		//BMP280_U32_t p = compensate_pressure();
		//BMP280_S32_t t = compensate_temperature();
		//printf("pression compensée :%d \r\n",p);
		//printf("temperature compensée :%d \r\n",t);


		/*
		 *	SEND TEMPERATURE
		 */

		/*
		int32_t get_t;
		get_t = BMP280_get_temperature();

		char value[20];
		int size = sprintf(value, "\r\n%ld\r\n", get_t);
		HAL_UART_Transmit(&huart1, (uint8_t*)value, size - 1, 1000);
		printf("send temperature %s \r\n ", value);

		HAL_Delay(1000);
		 */





		/*
		 * ENVOIE AVEC RASBERRY PI
		 */
		/*

		char value[] = "rom\r\n";
		HAL_UART_Transmit(&huart1, (uint8_t*)value, sizeof(value) - 1, 1000);
		printf("send temperature %s \r\n ", value);

		HAL_Delay(1000);
		 */


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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
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
