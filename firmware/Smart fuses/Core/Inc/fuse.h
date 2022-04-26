/*
 * fuse.h
 *
 *  Created on: 7 kwi 2022
 *      Author: Piotr Lesicki
 */

#ifndef INC_FUSE_H_
#define INC_FUSE_H_

#include "stm32l4xx_hal.h"
#include "spi.h"
#include "timer.h"
#include "include/etl/vector.h"

/*
 * checks n number of times for fuse, the number shouldn't be too big
 * commonly VN100..afasdfhas sth responds relatively fast
 */
#define FUSE_TIMEOUT 6

//using namespace etl;

enum struct SmartFuseState
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

enum struct SamplingMode
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
	 * PWM duty cykle:
	 * 0 to 1023 - 0% to 100% fill
	 */
	uint16_t duty_cykle[6];

	/*
	 * clamping currents of respective fuses
	 * first is the bottom clamp and second is the upper clamp
	 */
	pair<uint16_t, uint16_t> clamping_currents[6];

	/*
	 * there is more but this much is enough for now
	 */
};

class SmartFuse final
{
	public:
		SmartFuse(const GPIO_TypeDef *port, const uint32_t pin, const SPI_HandleTypeDef *hspi, const FusesSettings fuses_settings);

		SmartFuseState activeFuse(FuseNumber fuse);
		SmartFuseState deactivateFuse(FuseNumber fuse);
		SmartFuseState activeAllFuses(void);
		SmartFuseState deactivateAllFuses(void);

		uint16_t readFuseCurrent(FuseNumber fuse);

		SmartFuseState init(void);
		SmartFuseState handle(void);

		SmartFuseState setFuseDutyCykle(FuseNumber fuse, uint16_t duty_cykle);

		SmartFuseState getSmartFuseState(void);

	private:
		struct Fuse;

		/*
		 * smart fuse has a watch dog timer toggle bit which needs to be toggled within
		 * a certain amount of time
		 */
		bool toggle;

		/*
		 * global state bit
		 * is read at each spi communication
		 */
		uint8_t last_gsb;

		const uint32_t pin;

		Fuse fuses[6];

		const GPIO_TypeDef *port;

		const SPI_HandleTypeDef *hspi;

		constexpr FusesSettings fuses_settings;

		Timer watch_dog;

		SmartFuseState state;

		void slaveSelect();
		void slaveDeselect();
		void transmitReceiveData(uint8_t *tx_data, uint8_t *rx_data);

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
			etl::pair<uint16_t, uint16_t> clamping_currents;
		};
};

template <int num_of_sf>
class SmartFuseHandler final
{
	public:
		etl::vector<SmartFuse*, num_of_sf> smart_fuses;

		/*
		 * true - if ok
		 * false - if not ok
		 */
		bool handle(void);

	private:

};

#endif /* INC_FUSE_H_ */
