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

	#include "string.h"
	#include "stdio.h"
	#include "local_config.h"
	#include "yx5200_sm.h"
	#include "tm1637_sm.h"
	#include "debug_gx.h"

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

	volatile  uint8_t	   time_to_beep_u8 = 0 ;		// base on TIm3
	static uint16_t time_counter_u16 = 0;
	tm1637_struct 	h1_tm1637;
	yx5200_struct 	h1_yx5200;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	DWT_Delay_Init();
	//PC13_HotFix();
	DebugSoftVersion(SOFT_VERSION);
	DBG1("\t BaGod-19 & stm32F103\r\n");
	DBG1("\t   board: JM-Electron\r\n ");
	DBG2("\t debugTx: PA9, UART1, 62500\r\n");
	DBG3("\t USART1->BRR: %lx\r\n", USART1->BRR);
	DBG2("\t     LED: PA1 (need Push Pull) \r\n");

	h1_yx5200.uart		= &MP3_UART;
	h1_yx5200.busy_pin	= MP3_BUSY_Pin;
	h1_yx5200.busy_port	= MP3_BUSY_GPIO_Port;

	h1_tm1637.clk_pin  = TM1637_CLK_Pin;
	h1_tm1637.clk_port = TM1637_CLK_GPIO_Port;
	h1_tm1637.dio_pin  = TM1637_DIO_Pin;
	h1_tm1637.dio_port = TM1637_DIO_GPIO_Port;
	h1_tm1637.digit_qnt= 4;	// or 4 for 4 digits
	TM1637_Init(&h1_tm1637);
	TM1637_Set_Brightness(&h1_tm1637, bright_15percent);
	TM1637_Display_Decimal(&h1_tm1637, 1234, double_dot, symbol_dec);

	//__HAL_RCC_GPIOB_CLK_ENABLE();
	MP3_YX5200_Init(&h1_yx5200);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
	HAL_Delay(2000);

	for (int i= 0; i<17; i++) {
		DBG1("%d ", 16-i );  fflush(stdout);
		TM1637_Display_Decimal(&h1_tm1637, 16-i, no_double_dot, symbol_dec);
		MP3_YX5200_Play_with_index(&h1_yx5200, i);
	}

	DBG1("\r\n3 ...\r\n");
	TM1637_Display_Decimal(&h1_tm1637, 3, no_double_dot, symbol_dec);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(1000);

	DBG1("2 ..\r\n");
	TM1637_Display_Decimal(&h1_tm1637, 2, no_double_dot, symbol_dec);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(1000);

	MP3_YX5200_Play_with_index(&h1_yx5200, 18);

	DBG1("Start\r\n");

	TM1637_Display_Decimal(&h1_tm1637, 1, no_double_dot, symbol_dec);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);

	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	if (time_to_beep_u8 == 1 ) {
		time_to_beep_u8 = 0;
		uint16_t BaGod_min = time_counter_u16 / 6 ;
		uint16_t BaGod_sec = time_counter_u16 % 6 ;

		DBG1("counter=%d %dhv %dsec\r\n", (int)time_counter_u16, (int)BaGod_min, (int)(10*BaGod_sec));
		TM1637_Display_Decimal(&h1_tm1637, (BaGod_min*100 + BaGod_sec*10), double_dot, symbol_dec);

		for (uint32_t j=0; j<BaGod_min; j++) {
			DBG1("min= %d\r\n", (int)j );
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
			HAL_Delay(300);
		}

		HAL_Delay(300);
		for (uint32_t i=0; i<BaGod_sec; i++) {
			DBG1("sec= %d\r\n", (int)i );
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
			HAL_Delay(10);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
			HAL_Delay(300);
		}

		time_counter_u16++;

		if (time_counter_u16 >= 19) time_counter_u16 = 1;

		MP3_YX5200_Play_with_index(&h1_yx5200, 13 + BaGod_min);
		MP3_YX5200_Play_with_index(&h1_yx5200, 7 + BaGod_sec);
		MP3_YX5200_Play_with_index(&h1_yx5200, 17);
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
