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
#include "PUTM_EV_CAN_LIBRARY/lib/can_interface.hpp"

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

void initCAN();

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

CAN_FilterTypeDef can_filtering_config;

//std::array < bool, 5 > states;

std::array < SmartFuseState, 4 > states;
std::array < std::array < FuseState, 6 >, 4 > chanel_states;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	SmartFuseHandler<4> sf_handler;

	uint8_t _1 = 0x2;

	FusesSettings fuses_settings
	{
		.active = { true, true, true, true, true, true },
		.latch_off_time_out = { _1, _1, _1, _1, _1, _1 },
		.sampling_mode = { SamplingMode::Continuous, SamplingMode::Continuous, SamplingMode::Continuous,
							SamplingMode::Continuous, SamplingMode::Continuous, SamplingMode::Continuous },
		.duty_cycle = { 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff, 0x03ff },
		.clamping_currents = { { 0x0000, 0xffff },  { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff }, { 0x0000, 0xffff } }
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
	initCAN();

	led_ok.deactivate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.deactivate();

	std::array < GpioInElement, 5 > optos { safety_ams, safety_spare, safety_tms, safety_td, safety_hvd };

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	sf_handler.enableAll();

	enable_mosfets.activate();

	led_ok.activate();

	while (1)
	{
		//----------------------------------------------------------------------------------------
		// handle smart fuses and show as Ok/Warnings/Error
		auto state = sf_handler.handleAll();
		switch (state)
		{
			case SmartFuseState::LatchOff: led_warning_1.activate(); break;
			case SmartFuseState::ResetState: led_warning_2.activate(); break;
			case SmartFuseState::OLOFF: led_warning_2.activate(); break;
			case SmartFuseState::OTPLVDS: led_warning_2.activate(); break;
			case SmartFuseState::TempFail: led_warning_2.activate(); break;
			case SmartFuseState::NotResponding: led_error.activate(); break;
			case SmartFuseState::SPIError: led_error.activate(); break;
			case SmartFuseState::FailSafe: led_error.activate(); break;
			default:
			{
				led_warning_1.deactivate();
				led_warning_2.deactivate();
				led_error.deactivate();
			}
		}
		// debug stuff
		states = sf_handler.getStates();
		chanel_states = sf_handler.getChanelsStates();

		//----------------------------------------------------------------------------------------
		// handle safety
		for (auto& safety : optos)
		{
			safety.isActive();
		}

		//----------------------------------------------------------------------------------------
		// transmit receive can and handle

		auto device_state = PUTM_CAN::SF_states::OK;
		for(size_t i = 0; i < 4; i++)
		{
			if(states[i] != SmartFuseState::Ok)
				device_state = static_cast<PUTM_CAN::SF_states>(i);
		}

		uint16_t current_sum = 0;

		for(auto& sf : sf_handler.smart_fuses)
		{
			for(size_t i = 0; i < 6; i++)
			{
				current_sum += sf.getFuseCurrent(static_cast<FuseNumber>(i));
			}
		}

		PUTM_CAN::SF_main sf_main
		{
			{ 1, 1, 1, 1, 500 },
			device_state
		};

		PUTM_CAN::Can_tx_message<PUTM_CAN::SF_main> can_sender(sf_main, PUTM_CAN::can_tx_header_SF_MAIN);
		if(can_sender.send(hcan1) != HAL_StatusTypeDef::HAL_OK) led_error.activate();
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void initCAN()
{
//	can_filtering_config.FilterBank = 0;
//	can_filtering_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	can_filtering_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	can_filtering_config.FilterIdHigh = 0x0000;
//	can_filtering_config.FilterIdLow = 0x0000;
//	can_filtering_config.FilterMaskIdHigh = 0x0000;
//	can_filtering_config.FilterMaskIdLow = 0x0000;
//	can_filtering_config.FilterFIFOAssignment = CAN_RX_FIFO0;
//	can_filtering_config.FilterActivation = ENABLE;
//	can_filtering_config.SlaveStartFilterBank = 0;//14;
//
//	if (HAL_CAN_ConfigFilter(&hcan1, &can_filtering_config) != HAL_OK)
//		Error_Handler();
//
//	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
//		Error_Handler();
//
//	if ( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK )
//		Error_Handler();
//
//	if (HAL_CAN_Start(&hcan1) != HAL_OK)
//		Error_Handler();
	  CAN_FilterTypeDef sFilterConfig;
	  sFilterConfig.FilterBank = 0;
	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  sFilterConfig.FilterIdHigh = 0x0000;
	  sFilterConfig.FilterIdLow = 0x0000;
	  sFilterConfig.FilterMaskIdHigh = 0x0000;
	  sFilterConfig.FilterMaskIdLow = 0x0000;
	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  sFilterConfig.FilterActivation = ENABLE;


		if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) {
			Error_Handler();
		}

		if (HAL_CAN_Start(&hcan1) != HAL_OK) {
			Error_Handler();
		}

		if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) {
			Error_Handler();
		}


		  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

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
