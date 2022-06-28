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
#include "adc.h"
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
HAL_StatusTypeDef sendCanFrameFrontBox();
HAL_StatusTypeDef sendCanFrameCoolingAndSafety();
HAL_StatusTypeDef sendCanFrameDV();
HAL_StatusTypeDef sendCanFrameDV();
HAL_StatusTypeDef sendCanFrameWS();
HAL_StatusTypeDef sendCanFrameNucs();
HAL_StatusTypeDef sendCanFrameSafety();

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

std::array < SmartFuseState, number_of_fuses > fuses_states;
std::array < std::array < ChannelState, number_of_channels_per_fuse >, number_of_fuses > channels_states;
std::array < std::array < uint16_t, number_of_channels_per_fuse >, number_of_fuses > channels_currents;
std::array < bool, 5 > safeties { };

SmartFuseHandler < number_of_fuses > sf_handler;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint8_t _1 = 0x2;

	ChannelSettings channel_setting
	{
		.active = true,
		.latch_off_time_out = _1,
		.sampling_mode = SamplingMode::Continuous,
		.duty_cycle = 0x3ff,
		.clamping_currents = { 0x0000, 0xffff }
	};

	std::array < ChannelSettings, number_of_channels_per_fuse > channels_settings
	{
		channel_setting,
		channel_setting,
		channel_setting,
		channel_setting,
		channel_setting,
		channel_setting
	};

	/*
	 * channel 0: inverter
	 * channel 1: front box
	 * channel 2: tsal/assi
	 * channel 3: motec
	 * channel 4: break light
	 * channel 5: fan mono
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_1, &hspi1, channels_settings);
	/*
	 * channel 0: wheel speed
	 * channel 1: dash
	 * channel 2: laptimer
	 * channel 3: fan l
	 * channel 4: fan r
	 * channel 5: odrive
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_2, &hspi1, channels_settings);
	/*
	 * channel 0: spare 1
	 * channel 1: asms/safety
	 * channel 2: lidar
	 * channel 3: wheel speed
	 * channel 4: box dv
	 * channel 5: jetson
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_3, &hspi1, channels_settings);
	/*
	 * channel 0: --
	 * channel 1: --
	 * channel 2: bat hv
	 * channel 3: spare 2
	 * channel 4: diagport
	 * channel 5: pump
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_4, &hspi1, channels_settings);

	sf_handler.smart_fuses[1].setActionInterval(100);
	sf_handler.smart_fuses[1].setAction([](SmartFuse* sf)
	{
		static uint16_t previous_setting = 1023;

		uint16_t setting = 0;

//		auto temps = PUTM_CAN::can.get_tc_temperatures();
//		if(temps.water_temp_in > 60) setting = 1023;
//		else if(temps.water_temp_in > 50) setting = 800;
//		else if(temps.water_temp_in > 40) setting = 500;
//		else setting = 0;

		auto tc_main = PUTM_CAN::can.get_tc_main();
		if(tc_main.traction_control_enable) setting = 0x3ff;
		else setting = 0;

		if(previous_setting != setting)
		{
			// fan left
			sf->setChannelDutyCykle(Channel::c3, setting);
			// fan right
			sf->setChannelDutyCykle(Channel::c4, setting);

			previous_setting = setting;
		}
	});

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
  MX_ADC1_Init();
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

	Timer timer_can_send_main_frame(10);
	Timer timer_can_send_other_frames(100);

	while (1)
	{
		//----------------------------------------------------------------------------------------
		// handle smart fuses and show as Ok/Warnings/Error
		auto state = sf_handler.handleAll();
		if(state != SmartFuseState::Ok) led_error.activate();
		else led_error.deactivate();
		// debug stuff
		fuses_states = sf_handler.getStates();
		channels_states = sf_handler.getChannelsStates();
		channels_currents = sf_handler.getChannelsCurrents();

		//----------------------------------------------------------------------------------------
		// handle safety
		for (size_t i = 0; i < optos.size(); i++)
		{
			safeties[i] = optos[i].isActive();
		}

		//----------------------------------------------------------------------------------------
		// transmit receive can and handle
		if(timer_can_send_main_frame.checkIfTimedOutAndReset())
		{
			auto device_state = PUTM_CAN::SF_states::OK;

			PUTM_CAN::FuseData fuses_overall_state { };

			for(size_t i = 0; i < 4; i++)
				if(fuses_states[i] != SmartFuseState::Ok)
					device_state = static_cast<PUTM_CAN::SF_states>(i + 2);

			for(auto& sf : channels_currents)
				for(auto& ch_current : sf)
					fuses_overall_state.current += ch_current;

			for(auto& sf : fuses_states)
				fuses_overall_state.ok |= (sf == SmartFuseState::Ok);

			PUTM_CAN::SF_main sf_main
			{
				.fuses_overall_state = fuses_overall_state,
				.device_state =	device_state
			};

			PUTM_CAN::Can_tx_message<PUTM_CAN::SF_main> can_sender(sf_main, PUTM_CAN::can_tx_header_SF_MAIN);

			if(can_sender.send(hcan1) != HAL_StatusTypeDef::HAL_OK) led_error.activate();
			else led_error.deactivate();
		}

		if(timer_can_send_other_frames.checkIfTimedOutAndReset())
		{
			auto can_ok = HAL_OK;

			if( sendCanFrameFrontBox() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameCoolingAndSafety() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameDV() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameDV() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameWS() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameNucs() != HAL_OK ) can_ok = HAL_ERROR;
			if( sendCanFrameSafety() != HAL_OK ) can_ok = HAL_ERROR;

			if(can_ok != HAL_OK) led_warning_2.activate();
			else led_warning_2.deactivate();
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 30;
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
	can_filtering_config.FilterBank = 0;
	can_filtering_config.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filtering_config.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filtering_config.FilterIdHigh = 0x0000;
	can_filtering_config.FilterIdLow = 0x0000;
	can_filtering_config.FilterMaskIdHigh = 0x0000;
	can_filtering_config.FilterMaskIdLow = 0x0000;
	can_filtering_config.FilterFIFOAssignment = CAN_RX_FIFO0;
	can_filtering_config.FilterActivation = ENABLE;
	can_filtering_config.SlaveStartFilterBank = 14;

	if ( HAL_CAN_ConfigFilter(&hcan1, &can_filtering_config) != HAL_OK )
		Error_Handler();

	if ( HAL_CAN_Start(&hcan1) != HAL_OK )
		Error_Handler();

	if ( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK )
		Error_Handler();
}

HAL_StatusTypeDef sendCanFrameFrontBox()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_FrontBox front_box
	{
		.fuse_0_inverter =
		{
			.ok = sf_buff[0].getChannelState(fuse_0_inverter) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[0].getChannelCurrent(fuse_0_inverter)
		},
		.fuse_0_boxf_mb =
		{
			.ok = sf_buff[0].getChannelState(fuse_0_front_box) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[0].getChannelCurrent(fuse_0_front_box)
		},
		.fuse_0_apps =
		{
			.ok = 0,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent =0,
			.current = 0
		},
		.fuse_0_tsal_logic =
		{
			.ok = sf_buff[0].getChannelState(fuse_0_tsal_assi) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[0].getChannelCurrent(fuse_0_tsal_assi)
		}
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_FrontBox> sender(front_box, PUTM_CAN::can_tx_header_SF_FRONTBOX);

	return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameCoolingAndSafety()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_CoolingAndVSafety cooling_and_safety
	{
		.fuse_1_fan_l =
		{
			.ok = sf_buff[1].getChannelState(fuse_1_fan_l) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[1].getChannelCurrent(fuse_0_inverter)
		},
		.fuse_1_fan_r =
		{
			.ok = sf_buff[1].getChannelState(fuse_1_fan_r) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[1].getChannelCurrent(fuse_1_fan_r)
		},
		.fuse_3_pump =
		{
			.ok = sf_buff[3].getChannelState(fuse_0_tsal_assi) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[3].getChannelCurrent(fuse_0_tsal_assi)
		},
		.fuse_2_v_safety =
		{
			.ok = sf_buff[2].getChannelState(fuse_0_tsal_assi) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[2].getChannelCurrent(fuse_0_tsal_assi)
		}
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_CoolingAndVSafety> sender(cooling_and_safety, PUTM_CAN::can_tx_header_SF_FRONTBOX);

	return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameDV()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_DV dv
	{
		.fuse_0_box_dv =
		{
			.ok = sf_buff[2].getChannelState(fuse_2_box_dv) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[2].getChannelCurrent(fuse_2_box_dv)
		},
		.fuse_0_tsal_hv =
		{
			.ok = sf_buff[0].getChannelState(fuse_0_tsal_assi) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[0].getChannelCurrent(fuse_0_tsal_assi)
		},
		.fuse_2_wheel =
		{
			.ok = sf_buff[1].getChannelState(fuse_1_wheel_speed_1) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[1].getChannelCurrent(fuse_1_wheel_speed_1)
		},
		.fuse_1_dashboard =
		{
			.ok = sf_buff[1].getChannelState(fuse_1_dash) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[1].getChannelCurrent(fuse_1_dash)
		}
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_DV> sender(dv, PUTM_CAN::can_tx_header_SF_DV);

	return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameWS()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_WS ws
	{
		.fuse_1_ws_rl =
		{
			.ok = sf_buff[1].getChannelState(fuse_1_wheel_speed_1) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[1].getChannelCurrent(fuse_1_wheel_speed_1)
		},
		.fuse_1_ws_fl =
		{
			.ok = 0,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = 0
		},
		.fuse_1_ws_rr =
		{
			.ok = 0,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = 0
		},
		.fuse_2_ws_fr =
		{
			.ok = sf_buff[2].getChannelState(fuse_2_wheel_speed_2) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[2].getChannelCurrent(fuse_2_wheel_speed_2)
		}
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_WS> sender(ws, PUTM_CAN::can_tx_header_SF_WS);

	return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameNucs()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_NUCS nucs
	{
		.fuse_2_jetson =
		{
			.ok = sf_buff[2].getChannelState(fuse_2_jetson) == ChannelState::Ok,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = sf_buff[2].getChannelCurrent(fuse_2_jetson)
		},
		.fuse_2_intel_nuc =
		{
			.ok = 0,
			.overheat = 0,
			.undercurrent = 0,
			.overcurrent = 0,
			.current = 0
		}
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_NUCS> sender(nucs, PUTM_CAN::can_tx_header_SF_NUCS);

	return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameSafety()
{
	auto& sf_buff = sf_handler.smart_fuses;

	PUTM_CAN::SF_safety safety
	{
		.firewall = safety_ams.isActive(),
		.hvd = safety_hvd.isActive(),
		.inverter = safety_spare.isActive(),
		.dv = safety_td.isActive(),
		.tsms = safety_tms.isActive()
	};

	PUTM_CAN::Can_tx_message<PUTM_CAN::SF_safety> sender(safety, PUTM_CAN::can_tx_header_SF_SAFETY);

	return sender.send(hcan1);
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
