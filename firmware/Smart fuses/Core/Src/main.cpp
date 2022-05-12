/*USER CODE BEGIN Header */
/**
 ******************************************************************************
 *@file           : main.c
 *@brief          : Main program body
 ******************************************************************************
 *@attention
 *
 *Copyright (c) 2022 STMicroelectronics.
 *All rights reserved.
 *
 *This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component.
 *If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/*USER CODE END Header */
/*Includes ------------------------------------------------------------------*/

#include "main.h"
#include "can.h"
#include "spi.h"
#include "gpio.h"

/*Private includes ----------------------------------------------------------*/
/*USER CODE BEGIN Includes */
#include "stm32l4xx_hal_spi.h"
#include "timer.h"
#include "fuse.hpp"
#include "gpio elements.hpp"

/*USER CODE END Includes */

/*Private typedef -----------------------------------------------------------*/
/*USER CODE BEGIN PTD */

/*USER CODE END PTD */

/*Private define ------------------------------------------------------------*/
/*USER CODE BEGIN PD */
#define LED_OK GPIOC, GPIO_PIN_0
#define LED_WARNING1 GPIOC, GPIO_PIN_1
#define LED_WARNING2 GPIOC, GPIO_PIN_2
#define LED_ERROR GPIOC, GPIO_PIN_3

/*USER CODE END PD */

/*Private macro -------------------------------------------------------------*/
/*USER CODE BEGIN PM */

/*USER CODE END PM */

/*Private variables ---------------------------------------------------------*/

/*USER CODE BEGIN PV */

Timer fuse_watch_dog;
Timer fuses_cotrol;

/*USER CODE END PV */

/*Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/*USER CODE BEGIN PFP */

//void selectFuse(int num);
//void deselectAllFuses();

//void sendDataToFuse(uint8_t, uint8_t *, uint8_t *);
//void calculateParityBit(uint8_t*);

/*USER CODE END PFP */

/*Private user code ---------------------------------------------------------*/
/*USER CODE BEGIN 0 */

/*USER CODE END 0 */

/**
 *@brief  The application entry point.
 *@retval int
 */
int main(void)
{
	/*USER CODE BEGIN 1 */

	SmartFuseHandler<4> sf_handler;

	FusesSettings fuses_settings
	{
		{ true, true, true, true, true, true },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ SamplingMode::Filtered, SamplingMode::Filtered, SamplingMode::Filtered,
		  SamplingMode::Filtered, SamplingMode::Filtered, SamplingMode::Filtered },
		{ 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff },
		{ { 0x0000, 0xffff },  { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff } }
	};

	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_1, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_2, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_3, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_4, &hspi1, fuses_settings);

	GpioOutElement led_ok(GPIOC, GPIO_PIN_0, true);
	GpioOutElement led_warning_1(GPIOC, GPIO_PIN_1, true);
	GpioOutElement led_warning_2(GPIOC, GPIO_PIN_2, true);
	GpioOutElement led_error(GPIOC, GPIO_PIN_3, true);

	/*USER CODE END 1 */

	/*MCU Configuration--------------------------------------------------------*/

	/*Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/*USER CODE BEGIN Init */

	/*USER CODE END Init */

	/*Configure the system clock */
	SystemClock_Config();

	/*USER CODE BEGIN SysInit */

	/*USER CODE END SysInit */

	/*Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CAN1_Init();
	MX_SPI1_Init();
	/*USER CODE BEGIN 2 */

	//sf_handler.init_all();
	sf_handler.smart_fuses[1].init();

	led_ok.activate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.deactivate();

	/*USER CODE END 2 */

	/*Infinite loop */
	/*USER CODE BEGIN WHILE */
	Timer tim;
	bool tog = false;
	SmartFuseState state;
	while (1)
	{
		sf_handler.smart_fuses[1].handle();
		state = sf_handler.smart_fuses[1].getSmartFuseState();
		if(state != SmartFuseState::Ok ) led_error.activate();


 		if(tim.getPassedTime() >= 5000)
		{
 			tim.restart();
			if (tog)
			{
				sf_handler.smart_fuses[1].activeAllFuses();
			}
			else
			{
				sf_handler.smart_fuses[1].deactivateAllFuses();
			}
			tog = !tog;
		}
		/*USER CODE END WHILE */

		/*USER CODE BEGIN 3 */
	}

	/*USER CODE END 3 */
}

/**
 *@brief System Clock Configuration
 *@retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/*
	 * Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/*
	 * Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/**Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/*USER CODE BEGIN 4 */

//void selectFuse(int num)
//{
//	HAL_GPIO_WritePin(FUSE0, (num == 1 ? RESET : SET));
//	HAL_GPIO_WritePin(FUSE1, (num == 2 ? RESET : SET));
//	HAL_GPIO_WritePin(FUSE2, (num == 3 ? RESET : SET));
//	HAL_GPIO_WritePin(FUSE3, (num == 4 ? RESET : SET));
//}

//void deselectAllFuses()
//{
//	HAL_GPIO_WritePin(FUSE0, SET);
//	HAL_GPIO_WritePin(FUSE1, SET);
//	HAL_GPIO_WritePin(FUSE2, SET);
//	HAL_GPIO_WritePin(FUSE3, SET);
//	//HAL_Delay(100);
//}

//void transferReceive(uint8_t *tx_data, uint8_t *rx_data)
//{
//	while(!__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE));
//	*(__IO uint8_t *) &(&hspi1)->Instance->DR = (*tx_data);
//	while(!__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE));
//	(*(uint8_t*) rx_data) = *(__IO uint8_t *) &(&hspi1)->Instance->DR;
//}

//void sendDataToFuse(uint8_t fuse, uint8_t *tx_data, uint8_t *rx_data)
//{
//	/// just check
//	if (((&hspi1)->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE) __HAL_SPI_ENABLE(&hspi1);
//
//	selectFuse(fuse);
//	for (uint8_t tx = 0, rx = 0; tx < 3 || rx < 3;)
//	{
//		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) && rx < 3)
//		{
//			(*(uint8_t*) rx_data) = *(__IO uint8_t *) &(&hspi1)->Instance->DR;
//			rx_data++;
//			rx++;
//		}
//
//		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) && tx < 3)
//		{
//			*(__IO uint8_t *) &(&hspi1)->Instance->DR = (*tx_data);
//			tx_data++;
//			tx++;
//		}
//	}
//	deselectAllFuses();
//}

//void calculateParityBit(uint8_t* x)
//{
//	///clear parity bit
//	*(x + 2) &= ~(1 << 0);
//
//	uint8_t num_of_bits = 0;
//	for (int i = 0; i < 3; i++)
//	{
//		uint8_t bit = *(x + i);
//		for (int j = 0; j < 8; j++)
//		{
//			if (bit & (1 << j)) num_of_bits++;
//		}
//	}
//	if(num_of_bits % 2 == 0 )
//	{
//		*(x + 2) |= (1 << 0);
//	}
//}

/*USER CODE END 4 */

/**
 *@brief  This function is executed in case of error occurrence.
 *@retval None
 */
void Error_Handler(void)
{
	/*USER CODE BEGIN Error_Handler_Debug */
	/*User can add his own implementation to report the HAL error return state */
	HAL_GPIO_WritePin(LED_OK, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_WARNING1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_WARNING2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_ERROR, GPIO_PIN_RESET);
	__disable_irq();
	while (1) {}

	/*USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 *@brief  Reports the name of the source file and the source line number
 *        where the assert_param error has occurred.
 *@param  file: pointer to the source file name
 *@param  line: assert_param error line source number
 *@retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/*USER CODE BEGIN 6 */
	/*User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/*USER CODE END 6 */
}
#endif /*USE_FULL_ASSERT */
