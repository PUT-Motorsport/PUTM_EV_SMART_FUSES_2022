/*
 * fuse.h
 *
 *  Created on: 7 kwi 2022
 *      Author: Piotr Lesicki
 *
 *			================================================
 *  		||  ABC of fuses library with PUT Motosport   ||
 *  		================================================
 *
 *  1. 	Unfortunately compiler for some reasons can mess up template compiling
 *  	so it became a necessity to hint compiler in the hpp file what types of templates
 *  	we need - at the bottom of this file there is a hinting section.
 *
 *  2.	Most of values have their explanation in place so definitely check those.
 *
 *  3.	Fuses use a handler object, which handles all fuses, which needs to be called every
 *  	loop iteration.
 *
 *  4.  Well settings are a mess i guess.
 *
 *  5. 	How to pro
 */

#ifndef INC_FUSE_HPP_
#define INC_FUSE_HPP_

#define SMART_FUSE_CUSTOM_SEND_ENABLE false

#include "stm32l4xx_hal.h"
#include "spi.h"
#include "timer.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "etl/delegate.h"

enum struct Channel : uint8_t
{
	c0,
	c1,
	c2,
	c3,
	c4,
	c5
};

/*
 * checks n number of times for fuse, the number shouldn't be too big
 * commonly VN100..afasdfhas sth responds relatively fast
 */
const size_t fuse_timeout = 6;

/*
 * number of channels per fuse. WARNING this constant should't be ever changed
 */
const size_t number_of_channels_per_fuse = 6;

/*
 * number of fuses being used in program / on board
 */
const size_t number_of_fuses = 4;

/*
 * channels name to number definitions,
 */

//fuse 0
const Channel fuse_0_inverter = Channel::c0;
const Channel fuse_0_front_box = Channel::c1;
const Channel fuse_0_tsal_assi = Channel::c2;
const Channel fuse_0_motec = Channel::c3;
const Channel fuse_0_break_light = Channel::c4;
const Channel fuse_0_fan_mono = Channel::c5;

//fuse 1
const Channel fuse_1_wheel_speed_1 = Channel::c0;
const Channel fuse_1_dash = Channel::c1;
const Channel fuse_1_lapimer = Channel::c2;
const Channel fuse_1_fan_l = Channel::c3;
const Channel fuse_1_fan_r = Channel::c4;
const Channel fuse_1_odrive = Channel::c5;

//fuse 2
const Channel fuse_2_spare_1 = Channel::c0;
const Channel fuse_2_asms_safety = Channel::c1;
const Channel fuse_2_lidar = Channel::c2;
const Channel fuse_2_wheel_speed_2 = Channel::c3;
const Channel fuse_2_box_dv = Channel::c4;
const Channel fuse_2_jetson = Channel::c5;

//fuse 3
const Channel fuse_3_bat_hv = Channel::c2;
const Channel fuse_3_spare_2 = Channel::c3;
const Channel fuse_3_diagport = Channel::c4;
const Channel fuse_3_pump = Channel::c5;

enum struct SmartFuseState : uint8_t
{
	Ok,
	ResetState,
	SPIError,
	/*
	 * over temperature event +
	 * power limitation event +
	 * VDS mask idk what it is xd
	 */
	OTPLVDS,
	TempFail,
	LatchOff,
	/*
	 * open load event  +
	 * shorted to ground
	 */
	OLOFF,
	/*
	 * device enters FailSafe on start up or when watch dog is
	 * failed to be toggled within every 30 ms to 70 ms
	 */
	FailSafe,
	NotResponding,
};

enum struct ChannelState : uint8_t
{
	Ok,
	UnderCurrent,
	OverCurrent,
	LatchOff,
	/*
	 *  Output stuck to VCC/openload off state status.
	 */
	STKFLTR,
	/*
	 * This bit is ‘1’ if VDS is high at turn-off,
	 * indicative of a potential overload condition
	 */
	VDSFS,
	/*
	 * Channel feedback status. Combination of Power limitation, OT,
	 * OVERLOAD detection (VDS at turn-off). This bit is latched
	 * during OFF-state of the channel in order to allow asynchronous
	 * diagnostic and it is automatically cleared when the PL/OT/VDS
	 * junction temperature falls below the thermal reset temperature
	 * of OT detection, TRS.
	 */
	CHFBSR,
	Error
};

enum struct SamplingMode : uint8_t
{
	Stop,
	Start,
	Continuous,
	Filtered
};

struct ChannelSettings
{
	bool active;

	/*
	 * how long it takes for latch-off event to reset to latch-on,
	 * the chip will try to restart the corresponding channel within
	 * the specified time out if the cause of the latch-off event
	 * isn't cleared in that time, the channel will stay latched-off
	 * 0x0 - stays latched off
	 * 0x1 / 0xF - 16ms to 240 ms
	 */
	uint8_t latch_off_time_out;

	/*
	 * SamplingMode - there are 4 configurations:
	 * Stop,
	 * Start, (fires once)
	 * Continuous,
	 * Filtered (uses low pass filter)
	 */
	SamplingMode sampling_mode;

	/*
	 * PWM duty cycle:
	 * 0 to 1023 - 0% to 100% fill
	 */
	uint16_t duty_cycle;

	/*
	 * clamping currents of respective fuses
	 * first is the bottom clamp and second is the upper clamp
	 */
	std::pair<uint16_t, uint16_t> clamping_currents;

	/*
	 * there is more but this much is enough for now
	 */
};

class SmartFuse
{
	public:
		SmartFuse(const GPIO_TypeDef* const,
				  const uint32_t,
				  const SPI_HandleTypeDef* const,
				  std::array < ChannelSettings, number_of_channels_per_fuse >);

		void setActionInterval(uint32_t);
		/*
		 * arg 1: sender
		 */
		void setAction(etl::delegate<void(SmartFuse*)>);

		uint8_t getLastGSB() const;

		uint16_t getChannelCurrent(Channel);

		SmartFuseState activeChannel(Channel);
		SmartFuseState deactivateChannel(Channel);
		SmartFuseState activeAllChannels();
		SmartFuseState deactivateAllChannels();

		SmartFuseState init();
		/*
		 * shouldn't be used outside of handler if there is more than two
		 * smart fuses, because it can cause some inconsistencies in timing of
		 * SmarFuses watch dogs
		 */
		SmartFuseState enable();
		SmartFuseState disable();
		/*
		 * handles the smart fuse watchdog, current read / clamping, also if for
		 * any reason, a channel was switched off, program assumes that the channel
		 * was shorted to ground
		 */
		SmartFuseState handle();
		SmartFuseState handleTimer();

		/*
		 * set dutyCycle on specified channel
		 */
		SmartFuseState setChannelDutyCykle(Channel, uint16_t);

		SmartFuseState getState() const;

#if SMART_FUSE_CUSTOM_SEND_ENABLE
		/*
		 * send custom command, shoulnd't be used unless necessary
		 * arg 1: tx_data - data to send
		 * arg 2: ref rx_data - data recived from smart fuse
		 */
		SmartFuseState transmitReceiveCustomCommand(std::array < uint8_t, 3 >, std::array < uint8_t, 3 >&);
#endif
		ChannelState getChannelState(Channel);

		std::array < ChannelState, number_of_channels_per_fuse > getChannelsStates();
		std::array < uint16_t, number_of_channels_per_fuse > getChannelsCurrents();

	private:
		/*
		 * helps the management of fuses
		 */
		struct ChannelSettingsAndData
		{
			ChannelSettingsAndData();

			bool active;

			uint8_t latch_off_time_out;

			uint16_t duty_cycle;
			uint16_t current;

			std::pair<uint16_t, uint16_t> clamping_currents;

			SamplingMode sampling_mode;

			ChannelState state;
		};

		/*
		 * smart fuse has a watch dog timer toggle bit which needs to be toggled within
		 * a certain amount of time. It's handled by handle() and handleTimer()
		 */
		bool toggle;

		bool action_defined;

		/*
		 * global state bit
		 * is read at each spi communication
		 */
		uint8_t last_gsb;

		const uint32_t pin;

		std::array < ChannelSettingsAndData, number_of_channels_per_fuse > channels;

		const GPIO_TypeDef* const port;

		const SPI_HandleTypeDef* const hspi;

		//ChannelsSettings channels_settings;

		Timer watch_dog;
		Timer action_timer;

		SmartFuseState state;

		etl::delegate<void(SmartFuse*)> action;

		void slaveSelect();
		void slaveDeselect();

		void reset();
		void setUpAllDutyCycles();
		void setUpAllSamplingModes();
		void setUpAllLatchOffTimers();
		void setUpAllChannelsStates();

		void transmitReceiveData(std::array < uint8_t, 3 >, std::array < uint8_t, 3 >&);

		SmartFuseState getGSB(std::array < uint8_t, 3 > x);
};

template <uint32_t num_of_sf>
class SmartFuseHandler
{
	public:
	etl::vector < SmartFuse, num_of_sf > smart_fuses;

		/*
		 * just passes args to a private vector's emplace_back
		 */
		void emplaceBack(const GPIO_TypeDef * const, const uint32_t, const SPI_HandleTypeDef *const, std::array < ChannelSettings, number_of_channels_per_fuse >);

		/*
		 * all functions return summed up states
		 */
		SmartFuseState handleAll();
		SmartFuseState initAll();
		/*
		 * halting function - num_of_fuses * 5 ms
		 * it's there prevent watchdog time frames overlapping, which caused
		 * some timing issues.
		 */
		SmartFuseState enableAll();
		SmartFuseState disableAll();

		std::array < SmartFuseState, num_of_sf > getStates();
		std::array < std::array < ChannelState, number_of_channels_per_fuse >, num_of_sf > getChannelsStates();
		std::array < std::array < uint16_t, number_of_channels_per_fuse >, num_of_sf > getChannelsCurrents();
		//const etl::vector < SmartFuse, num_of_sf >& getSmartFuses() const;
		//const SmartFuse& getSmartFuse(uint8_t) const;

	private:
		//etl::vector < SmartFuse, num_of_sf > smart_fuses;
};

/*
 * template hinting section
 */

/// hinting compiler that we need this template cause it wouldn't work without it
template class SmartFuseHandler<number_of_fuses>;

#endif /* INC_FUSE_HPP_ */
