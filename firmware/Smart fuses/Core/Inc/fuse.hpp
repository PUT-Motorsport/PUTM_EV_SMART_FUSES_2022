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
 *  3.	Fuses use a handler object, which handles all fuses, which needs to be called.
 *
 *  4.  Well settings are a mess i guess.
 *
 *  5. 	How to pro
 */

#ifndef INC_FUSE_HPP_
#define INC_FUSE_HPP_

#include "stm32l4xx_hal.h"
#include "spi.h"
#include "timer.h"
#include "etl/array.h"
#include "etl/vector.h"

/*
 * checks n number of times for fuse, the number shouldn't be too big
 * commonly VN100..afasdfhas sth responds relatively fast
 */

constexpr size_t fuse_timeout = 6;

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
	 * failed to be toggled within 30 ms to 70 ms
	 */
	FailSafe,
	NotResponding,
};

enum struct FuseState : uint8_t
{
	Ok,
	UnderCurrent,
	OverCurrent,
	ShortedToGround,
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

enum struct FuseNumber : uint8_t
{
	f0,
	f1,
	f2,
	f3,
	f4,
	f5
};

struct FusesSettings
{
	bool active[6];

	/*
	 * how long it takes for latch-off event to reset to latch-on
	 * 0x0 - stays latched off
	 * 0x1 / 0xF - 16ms to 240 ms
	 */
	uint8_t latch_off_time_out[6];

	/*
	 * SamplingMode - there are 4 configurations:
	 * Stop,
	 * Start,
	 * Continuous,
	 * Filtered
	 */
	SamplingMode sampling_mode[6];

	/*
	 * PWM duty cycle:
	 * 0 to 1023 - 0% to 100% fill
	 */
	uint16_t duty_cycle[6];

	/*
	 * clamping currents of respective fuses
	 * first is the bottom clamp and second is the upper clamp
	 */
	std::pair<uint16_t, uint16_t> clamping_currents[6];

	/*
	 * there is more but this much is enough for now
	 */
};

class SmartFuse
{
	public:
		SmartFuse(const GPIO_TypeDef * const port, const uint32_t pin, const SPI_HandleTypeDef *const hspi, const FusesSettings &fuses_settings);

		SmartFuseState activeFuse(FuseNumber fuse);
		SmartFuseState deactivateFuse(FuseNumber fuse);
		SmartFuseState activeAllFuses();
		SmartFuseState deactivateAllFuses();

		uint16_t getFuseCurrent(FuseNumber fuse);

		SmartFuseState init();
		/*
		 * shouldn't be used outside of handler if there is more than two
		 * smartfuses, because it can couse some inconsistencies in timing of
		 * SmarFuses watch dogs
		 */
		SmartFuseState enable();
		SmartFuseState disable();
		/*
		 * handles the smart fuse watchdog, current read / clamping, also if for
		 * some reason, a channel was switched off, program assumes that the channel
		 * was shorted to ground
		 */
		SmartFuseState handle();
		SmartFuseState handleTimer();

		SmartFuseState setFuseDutyCykle(FuseNumber fuse, uint16_t duty_cycle);

		SmartFuseState getState() const;

		std::array < FuseState, 6 > getFuseStates();

		uint8_t getLastGSB() const;

	private:
		/*
		 * helps the management of fuses
		 */
		struct Fuse
		{
			Fuse();

			bool active;

			uint16_t current;

			/*
			 * clamping currents
			 */
			std::pair < uint16_t, uint16_t > clamping_currents;

			FuseState state;
		};

		/*
		 * smart fuse has a watch dog timer toggle bit which needs to be toggled within
		 * a certain amount of time. It's handled by handle() and handleTimer()
		 */
		bool toggle;

		/*
		 * global state bit
		 * is read at each spi communication
		 */
		uint8_t last_gsb;

		const uint32_t pin;

		std::array < Fuse, 6 > fuses;

		const GPIO_TypeDef * const port;

		const SPI_HandleTypeDef * const hspi;

		const FusesSettings fuses_settings;

		Timer watch_dog;

		SmartFuseState state;

		void slaveSelect();
		void slaveDeselect();

		void reset();
		void setUpAllDutyCycles();
		void setUpAllSamplingModes();
		void setUpAllLatchOffTimers();
		void setUpAllChanelsStates();

		void transmitReceiveData(std::array < uint8_t, 3 > tx_data, std::array < uint8_t, 3 > &rx_data);

		SmartFuseState getGSB(std::array < uint8_t, 3 > x);
};

template <uint32_t num_of_sf>
class SmartFuseHandler
{
	public:

		etl::vector < SmartFuse, num_of_sf > smart_fuses;

		/*
		 * all functions return summed up states
		 */
		SmartFuseState handleAll();
		SmartFuseState initAll();
		/*
		 * halting function - num_of_fuses * 5 ms
		 * it's there prevent watchdog time frames overlapping, which coused
		 * some timing issues.
		 */
		SmartFuseState enableAll();
		SmartFuseState disableAll();

		std::array < SmartFuseState, num_of_sf > getStates();
		std::array < std::array < FuseState, 6 >, num_of_sf > getChanelsStates();
};

/*
 * template hinting section
 */

/// hinting compiler that we need this template couse it wouldn't work without it
template class SmartFuseHandler<4>;

#endif /* INC_FUSE_HPP_ */
