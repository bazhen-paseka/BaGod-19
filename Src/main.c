/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

	#include <string.h>
	#include "yx5200_sm.h"
	#include "tm1637_sm.h"
	#include "stdio.h"

	volatile  uint8_t	   time_to_beep_u8 = 0 ;		// base on TIm3

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
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

	static uint16_t time_counter_u16 = 0;
	char DataChar[100];

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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	yx5200_struct h1_yx5200 =
	{
		.uart		= &huart2,
		.busy_port	= GPIOB,
		.busy_pin	= GPIO_PIN_5
	};

	tm1637_struct h1_tm1637 =
	{
		.clk_pin  = GPIO_PIN_1,
		.clk_port = GPIOB,
		.dio_pin  = GPIO_PIN_3,
		.dio_port = GPIOB
	};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	yx5200_init(&h1_yx5200);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	tm1637_Init(&h1_tm1637);
	tm1637_Set_Brightness(&h1_tm1637, bright_45percent);
	tm1637_Display_Decimal(&h1_tm1637, 1637, double_dot);
	//HAL_Delay(1000);

	sprintf(DataChar,"\r\nBaGod-19 v1.2.0\r\nUART1 for debug started on speed 115200\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	HAL_Delay(2000);

	for (uint16_t i= 0; i<17; i++)
	{
		tm1637_Display_Decimal(&h1_tm1637, i, no_double_dot);
		yx5200_play_with_index(&h1_yx5200, i);
	}

	sprintf(DataChar,"3 ...\r\n");
	tm1637_Display_Decimal(&h1_tm1637, 3, no_double_dot);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	HAL_Delay(190);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	HAL_Delay(190);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	HAL_Delay(190);
	HAL_Delay(1000);

	sprintf(DataChar,"2 ..\r\n");
	tm1637_Display_Decimal(&h1_tm1637, 2, no_double_dot);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	HAL_Delay(190);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
	HAL_Delay(190);

	HAL_Delay(1000);

	yx5200_play_with_index(&h1_yx5200, 18);

	sprintf(DataChar,"Start\r\n");

	tm1637_Display_Decimal(&h1_tm1637, 1, no_double_dot);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);

	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	if (time_to_beep_u8 == 1 )
	{
		time_to_beep_u8 = 0;

		uint16_t BaGod_min = time_counter_u16 / 6 ;
		uint16_t BaGod_sec = time_counter_u16 % 6 ;

		sprintf(DataChar,"counter=%d %dhv %dsec\r\n", (int)time_counter_u16, (int)BaGod_min, (int)(10*BaGod_sec));
		HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
		tm1637_Display_Decimal(&h1_tm1637, (BaGod_min*100 + BaGod_sec*10), double_dot);

		for (uint32_t j=0; j<BaGod_min; j++)
		{
			//sprintf(DataChar,"min= %d\r\n", (int)j );
			//HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
			HAL_Delay(300);
		}

		HAL_Delay(300);
		for (uint32_t i=0; i<BaGod_sec; i++)
		{
			//sprintf(DataChar,"sec= %d\r\n", (int)i );
			//HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, RESET);
			HAL_Delay(10);
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, SET);
			HAL_Delay(300);
		}

		time_counter_u16++;

		if (time_counter_u16 >= 19) time_counter_u16 = 1;

		yx5200_play_with_index(&h1_yx5200, 13 + BaGod_min);
		yx5200_play_with_index(&h1_yx5200, 7 + BaGod_sec);
		yx5200_play_with_index(&h1_yx5200, 17);
	}

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

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
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
