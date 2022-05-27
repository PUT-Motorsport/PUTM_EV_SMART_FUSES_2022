 /* USER CODE BEGIN Header */
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l4xx_hal_spi.h"
#include "timer.h"
#include "fuse.hpp"
#include "gpio elements.hpp"


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

GpioOutElement led_ok(GPIOC, GPIO_PIN_0, true);
GpioOutElement led_warning_1(GPIOC, GPIO_PIN_1, true);
GpioOutElement led_warning_2(GPIOC, GPIO_PIN_2, true);
GpioOutElement led_error(GPIOC, GPIO_PIN_3, true);

GpioOutElement led_1_control(GPIOB, GPIO_PIN_5, false);
GpioOutElement led_2_control(GPIOB, GPIO_PIN_7, false);
GpioOutElement buzzer_control(GPIOB, GPIO_PIN_10, false);
GpioOutElement enable_mosfets(GPIOB, GPIO_PIN_9, false);

GpioInElement safety_ams(GPIOB, GPIO_PIN_0, true);
GpioInElement safety_spare(GPIOB, GPIO_PIN_1, true);
GpioInElement safety_tms(GPIOB, GPIO_PIN_2, true);
GpioInElement safety_td(GPIOB, GPIO_PIN_4, true);
GpioInElement safety_hvd(GPIOB, GPIO_PIN_6, true);

std::array < bool, 5 > states;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	SmartFuseHandler<4> sf_handler;

	FusesSettings fuses_settings
	{
		{ true, true, true, true, true, true },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ SamplingMode::Continuous, SamplingMode::Continuous, SamplingMode::Continuous,
		  SamplingMode::Continuous, SamplingMode::Continuous, SamplingMode::Continuous },
		{ 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff },
		{ { 0x0000, 0xffff },  { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff } }
	};

	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_1, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_2, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_3, &hspi1, fuses_settings);
	sf_handler.smart_fuses.emplace_back(GPIOA, GPIO_PIN_4, &hspi1, fuses_settings);

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
  MX_CAN1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

	sf_handler.initAll();

	led_ok.deactivate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.deactivate();

	std::array < GpioOutElement, 4 > leds { led_ok, led_warning_1, led_warning_2, led_error };
	std::array < GpioInElement, 5 > optos { safety_ams, safety_spare, safety_tms, safety_td, safety_hvd };

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	sf_handler.enableAll();
	enable_mosfets.activate();

	while (1)
	{
		sf_handler.handleAll();

		SF_main sf_test
		{
			{
				.ok = 1,
				.overheat = 0,
				.undercurrent = 0,
				.overcurrent = 0,
				.current = 0
			},
			SF_states::OK
		};

		auto sf_main_frame = PUTM_CAN::Can_tx_message<SF_main>(sf_test, can_tx_header_SF_MAIN);
		auto status = sf_main_frame.send(hcan1);
		if(status != HAL_StatusTypeDef::HAL_OK) led_error.activate();
		else led_error.deactivate();


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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
	/*User can add his own implementation to report the HAL error return state */
	led_ok.deactivate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.activate();

	__disable_irq();
	while (1) {}

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
	/*User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
