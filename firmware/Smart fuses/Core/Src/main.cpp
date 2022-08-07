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
#include "math.h"
#include "algorithm"
#include "etl/queue.h"

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

class Device
{
	public:
		static void raise(PUTM_CAN::SF_states);
		static void clear(PUTM_CAN::SF_states);
		static void handleState();

	private:
		inline static PUTM_CAN::SF_states state = PUTM_CAN::SF_states::Ok;
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void initCAN();

HAL_StatusTypeDef sendCanFramePassiveElements();
HAL_StatusTypeDef sendCanFrameLegendaryDVAndSupply();
HAL_StatusTypeDef sendCanFrameSupply();
HAL_StatusTypeDef sendCanFrameSafety();


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

const bool use_tc_main_ts_on_for_fan_control = true;

const size_t safety_sapre_2 = 0;
const size_t safety_sapre_3 = 1;
const size_t safety_hv = 2;
const size_t safety_hvd = 3;
const size_t safety_inverter = 4;
const size_t safety_firewall = 5;
const size_t safety_dv = 6;
const size_t safety_sapre_1 = 7;

bool water_pot_kek;

GpioOutElement led_error(GPIOC, GPIO_PIN_0, true);
GpioOutElement led_warning_2(GPIOC, GPIO_PIN_1, true);
GpioOutElement led_warning_1(GPIOC, GPIO_PIN_2, true);
GpioOutElement led_ok(GPIOC, GPIO_PIN_3, true);

GpioOutElement led_1_control(GPIOB, GPIO_PIN_5, false);
GpioOutElement led_2_control(GPIOB, GPIO_PIN_7, false);
GpioOutElement buzzer_control(GPIOB, GPIO_PIN_10, false);

GpioOutElement water_pot_enable(GPIOB, GPIO_PIN_2, true);

GpioInElement water_pot_state(GPIOB, GPIO_PIN_4, false);

GpioInElement spare_2_sense_sig(GPIOC, GPIO_PIN_6, false);
GpioInElement spare_3_sense_sig(GPIOC, GPIO_PIN_7, false);
GpioInElement hv_sense_sig(GPIOC, GPIO_PIN_8, false);
GpioInElement hvd_sense_sig(GPIOC, GPIO_PIN_9, false);
GpioInElement inverter_sense_sig(GPIOC, GPIO_PIN_10, false);
GpioInElement firewall_sense_sig(GPIOC, GPIO_PIN_11, false);
GpioInElement dv_sense_sig(GPIOC, GPIO_PIN_12, false);
GpioInElement spare_1_sense_sig(GPIOC, GPIO_PIN_13, false);

CAN_FilterTypeDef can_filtering_config;

//debug stuff
std::array < SmartFuseState, number_of_fuses > fuses_states;
std::array < std::array < ChannelState, number_of_channels_per_fuse >, number_of_fuses > channels_states;
std::array < std::array < uint16_t, number_of_channels_per_fuse >, number_of_fuses > channels_currents;
std::array < bool, 8 > safeties { };
std::array < HAL_StatusTypeDef, 6 > frame_send_fail { };

PUTM_CAN::SF_states device_state;

SmartFuseHandler < number_of_fuses > sf_handler;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	Device::init();

	ChannelSettings std_channel_setting
	{
		.active = true,
		.latch_off_time_out = 0x2,
		.sampling_mode = SamplingMode::Continuous,
		.duty_cycle = 0x3ff,
		/*
		 * VN9D30Q100F's ADC has 10-bit precision and can measure from 0 A to 31,5 A
		 * proportion 31,5 A / 1023 = 5A / x gives around 162,380... ~ 163 = 0x00a3
		 */
		.clamping_currents = { 0x0000, 0x00a3 }
	};

	std::array < ChannelSettings, number_of_channels_per_fuse > std_fuse_channels_settings
	{
		std_channel_setting,
		std_channel_setting,
		std_channel_setting,
		std_channel_setting,
		std_channel_setting,
		std_channel_setting
	};

	//----------------------------------------------------------------------------------------
	/*
	 * fuse 0
	 * channel 0: inverter
	 * channel 1: front box
	 * channel 2: tsal/assi
	 * channel 3: motec
	 * channel 4: break light
	 * channel 5: fan mono
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_1, &hspi1, std_fuse_channels_settings);

	//----------------------------------------------------------------------------------------
	/*
	 * fuse 1
	 * channel 0: wheel speed
	 * channel 1: dash
	 * channel 2: laptimer
	 * channel 3: fan l
	 * channel 4: fan r
	 * channel 5: odrive
	 * action: fan control
	 */
	std::array < ChannelSettings, number_of_channels_per_fuse > fuse_1_channels_settings
	{
		std_channel_setting,
		std_channel_setting,
		std_channel_setting,
		{
			// fan l
			.active = true,
			.latch_off_time_out = 0x2,
			.sampling_mode = SamplingMode::Continuous,
			.duty_cycle = 0x000,
			.clamping_currents = { 0x0000, 0x00a3 }
		},
		{
			// fan r
			.active = true,
			.latch_off_time_out = 0x2,
			.sampling_mode = SamplingMode::Continuous,
			.duty_cycle = 0x000,
			.clamping_currents = { 0x0000, 0x00a3 }
		},
		std_channel_setting
	};
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_2, &hspi1, fuse_1_channels_settings, 100, [](SmartFuse* sf)
	{

		static uint16_t previous_setting = 0;

		uint16_t setting = 0;

		if constexpr(!use_tc_main_ts_on_for_fan_control)
		{
			/*
			 * min difference for for the change in fan speed to occur
			 */
			constexpr uint16_t min_diff = 10;
			/*
			 * interpolation results for: (20, 0), (30, 200), (40, 500), (50, 800), (60, 1023)
			 */
			constexpr float a = 0.0328571429f;
			constexpr float b = 23.8314285714f;
			constexpr float c = - 507.8f;


			auto tc_inv_temp = static_cast<float>(PUTM_CAN::can.get_tc_temperatures().inverter);
			auto result = a * tc_inv_temp * tc_inv_temp + b * tc_inv_temp + c;
			setting = static_cast<uint16_t>(etl::clamp(result, 0.f, 1023.f));

			if(std::abs(previous_setting - setting) >= min_diff)
			{
				// fan left
				sf->setChannelDutyCykle(Channel::c3, setting);
				// fan right
				sf->setChannelDutyCykle(Channel::c4, setting);

				previous_setting = setting;
			}
		}
		else
		{
			auto tc_on = PUTM_CAN::can.get_tc_main().tractive_system_on;

			if(tc_on) setting = 1023;
			else setting = 0;

			if(setting != previous_setting)
			{
				// fan left
				sf->setChannelDutyCykle(Channel::c3, setting);
				// fan right
				sf->setChannelDutyCykle(Channel::c4, setting);

				previous_setting = setting;
			}
		}
	});

	//----------------------------------------------------------------------------------------
	/*
	 * fuse 2
	 * channel 0: spare 1
	 * channel 1: asms/safety
	 * channel 2: lidar
	 * channel 3: wheel speed
	 * channel 4: box dv
	 * channel 5: jetson
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_3, &hspi1, std_fuse_channels_settings);

	//----------------------------------------------------------------------------------------
	/*
	 * fuse 3
	 * channel 0: --
	 * channel 1: --
	 * channel 2: bat hv
	 * channel 3: spare 2
	 * channel 4: diagport
	 * channel 5: pump
	 */
	sf_handler.emplaceBack(GPIOA, GPIO_PIN_4, &hspi1, std_fuse_channels_settings);
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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

	sf_handler.initAll();
	initCAN();
	water_pot_enable.activate();
	led_2_control.deactivate();
	led_1_control.deactivate();

	led_ok.deactivate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.deactivate();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	sf_handler.enableAll();

	led_ok.activate();

	Timer timer_can_send_main_frame(10);
	Timer timer_can_send_other_frames(25);

	std::array < GpioInElement, 8 > optos
	{
		spare_2_sense_sig,
		spare_3_sense_sig,
		hv_sense_sig,
		hvd_sense_sig,
		inverter_sense_sig,
		firewall_sense_sig,
		dv_sense_sig,
		spare_1_sense_sig
	};

	while (true)
	{
		//----------------------------------------------------------------------------------------
		// handle smart fuses and show as Ok/Warnings/Error
		Device::handleState();
		auto state = sf_handler.handleAll();
		if(state != SmartFuseState::Ok) Device::raise(PUTM_CAN::SF_states::Warning_1);
		else Device::clear(PUTM_CAN::SF_states::Warning_1);

		// debug stuff
		fuses_states = sf_handler.getStates();
		channels_states = sf_handler.getChannelsStates();
		channels_currents = sf_handler.getChannelsCurrents();

		//----------------------------------------------------------------------------------------
		// handle safety
		for (size_t i = 0; i < optos.size(); i++)
			safeties[i] = optos[i].isActive();

		//----------------------------------------------------------------------------------------
		// transmit receive can and handle
		// main frame
		if(timer_can_send_main_frame.checkIfTimedOutAndReset())
		{
			PUTM_CAN::SF_main sf_main
			{
				.device_state =	device_state,
				.fuse_0_state = static_cast<PUTM_CAN::SmartFuseState>(fuses_states[0]),
				.fuse_1_state = static_cast<PUTM_CAN::SmartFuseState>(fuses_states[1]),
				.fuse_2_state = static_cast<PUTM_CAN::SmartFuseState>(fuses_states[2]),
				.fuse_3_state = static_cast<PUTM_CAN::SmartFuseState>(fuses_states[3])
			};

			PUTM_CAN::Can_tx_message<PUTM_CAN::SF_main> can_sender(sf_main, PUTM_CAN::can_tx_header_SF_MAIN);

			if(can_sender.send(hcan1) != HAL_StatusTypeDef::HAL_OK)
				Device::raise(PUTM_CAN::SF_states::Error);
			else Device::clear(PUTM_CAN::SF_states::Error);
		}

		// other frames
		if(timer_can_send_other_frames.checkIfTimedOutAndReset())
		{
			auto can_state = HAL_OK;
			static size_t send_frame = 0;

			switch (send_frame)
			{
				case 0: can_state = sendCanFramePassiveElements(); break;
				case 1: can_state = sendCanFrameLegendaryDVAndSupply(); break;
				case 2: can_state = sendCanFrameSupply(); break;
				case 3: can_state = sendCanFrameSafety(); break;
			}

			frame_send_fail[send_frame] = can_state;

			if(can_state != HAL_OK)
				Device::raise(PUTM_CAN::SF_states::Warning_2);
			else Device::clear(PUTM_CAN::SF_states::Warning_2);

			send_frame++;
			if(send_frame > 5) send_frame = 0;
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

void Device::handleState()
{
	led_ok.deactivate();
	led_warning_1.deactivate();
	led_warning_2.deactivate();
	led_error.deactivate();

	switch (Device::state)
	{
		case PUTM_CAN::SF_states::Ok:
			led_ok.activate(); break;
		case PUTM_CAN::SF_states::Warning_1:
			led_warning_1.activate(); break;
		case PUTM_CAN::SF_states::Warning_2:
			led_warning_2.activate(); break;
		case PUTM_CAN::SF_states::Error:
			led_error.activate();
	}
}

void Device::raise(PUTM_CAN::SF_states state)
{
	if (int(state) >= int(Device::state)) return;
	Device::state = state;
}

void Device::clear(PUTM_CAN::SF_states state)
{
	if (state != Device::state) return;
	Device::state = PUTM_CAN::SF_states::Ok;
}

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

HAL_StatusTypeDef sendCanFramePassiveElements()
{
	auto& sf_buff = sf_handler.smart_fuses;
	auto wat_pot_state = (water_pot_state.isActive() ? PUTM_CAN::ChannelState::Ok : PUTM_CAN::ChannelState::Error);

		PUTM_CAN::SF_PassiveElements frame
		{
			.break_light = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_break_light)),
			.fan_mono = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_fan_mono)),
			.fan_l = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_fan_l)),
			.fan_r = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_fan_r)),
			.wheel_speed_1 = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_wheel_speed_1)), // idk which is left and which is right
			.wheel_speed_2 = static_cast<PUTM_CAN::ChannelState>(sf_buff[2].getChannelState(fuse_2_wheel_speed_2)), // idk which is left and which is right or front or whatever
			.water_potentiometer = wat_pot_state, // they are together
			.tsal_assi = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_tsal_assi)) // supply for leds ex.
		};

		PUTM_CAN::Can_tx_message<PUTM_CAN::SF_PassiveElements> sender(frame, PUTM_CAN::can_tx_header_SF_PASSIVEELEMENTS);

		return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameLegendaryDVAndSupply()
{
	auto& sf_buff = sf_handler.smart_fuses;

		PUTM_CAN::SF_LegendaryDVAndSupply frame
		{
			.lidar = static_cast<PUTM_CAN::ChannelState>(sf_buff[2].getChannelState(fuse_2_lidar)),
			.box_dv = static_cast<PUTM_CAN::ChannelState>(sf_buff[2].getChannelState(fuse_2_box_dv)),
			.jetson = static_cast<PUTM_CAN::ChannelState>(sf_buff[2].getChannelState(fuse_2_jetson)),
			.odrive = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_odrive)),
			.tsal = static_cast<PUTM_CAN::ChannelState>(ChannelState::Ok),
			.bspd_esb = static_cast<PUTM_CAN::ChannelState>(ChannelState::Ok),
			.spare_1 = static_cast<PUTM_CAN::ChannelState>(sf_buff[2].getChannelState(fuse_2_spare_1)),
			.spare_2 = static_cast<PUTM_CAN::ChannelState>(sf_buff[3].getChannelState(fuse_3_spare_2))
		};

		PUTM_CAN::Can_tx_message<PUTM_CAN::SF_LegendaryDVAndSupply> sender(frame, PUTM_CAN::can_tx_header_SF_LEGENDARYDVANDSUPPLY);

		return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameSupply()
{
	auto& sf_buff = sf_handler.smart_fuses;

		PUTM_CAN::SF_Supply frame
		{
			.inverter = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_inverter)),
			.front_box = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_front_box)),
			.dash = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_dash)),
			.laptimer = static_cast<PUTM_CAN::ChannelState>(sf_buff[1].getChannelState(fuse_1_lapimer)),
			.bat_hv = static_cast<PUTM_CAN::ChannelState>(sf_buff[3].getChannelState(fuse_3_bat_hv)),
			.diagport = static_cast<PUTM_CAN::ChannelState>(sf_buff[3].getChannelState(fuse_3_diagport)),
			.pomp = static_cast<PUTM_CAN::ChannelState>(sf_buff[3].getChannelState(fuse_3_pump)),
			.motec = static_cast<PUTM_CAN::ChannelState>(sf_buff[0].getChannelState(fuse_0_motec))
		};

		PUTM_CAN::Can_tx_message<PUTM_CAN::SF_Supply> sender(frame, PUTM_CAN::can_tx_header_SF_SUPPLY);

		return sender.send(hcan1);
}

HAL_StatusTypeDef sendCanFrameSafety()
{
		PUTM_CAN::SF_safety frame
		{
			.firewall = safeties[safety_firewall],
			.hvd = safeties[safety_hvd],
			.inverter = safeties[safety_inverter],
			.dv = safeties[safety_dv],
			/*
			 * TODO: tsms not present should be hv_sense i guess
			 */
			.tsms = safeties[safety_inverter]
		};

		PUTM_CAN::Can_tx_message<PUTM_CAN::SF_safety> sender(frame, PUTM_CAN::can_tx_header_SF_SAFETY);

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
