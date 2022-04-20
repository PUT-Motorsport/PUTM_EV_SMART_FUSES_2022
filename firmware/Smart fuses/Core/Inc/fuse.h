/*
 * fuse.h
 *
 *  Created on: 7 kwi 2022
 *      Author: pile
 */

#ifndef INC_FUSE_H_
#define INC_FUSE_H_

#include "stm32l4xx_hal.h"
#include "spi.h"
#include "timer.h"
#include "include/etl/vector.h"

using namespace etl;

enum FuseControlerState
{
	Ok,
	ResetState,
	SPIError,
	OTPLVDS,
	TempFail,
	LatchOff,
	OLOFF,
	FailSafe,
	TargetDoesntRespond,
};

enum SamplingMode
{
	Stop,
	Start,
	Continuous,
	Filtered
};

struct FusesSettings
{
	bool active[6];

	/*
	 * how long it takes for latch-off event to reset to latch on
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
	 * 0 / 1023 - 0% to 100% fill
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
		SmartFuse(GPIO_TypeDef *port, uint32_t pin, SPI_HandleTypeDef *hspi, FusesSettings& fuses_settings);

		void init(void);
		void handle(void);
		//void setClampingCurrent(uint16_t bottom, uint16_t up);

		void setFuseDutyCykle(uint8_t fuse, uint16_t duty_cykle);
		void activeFuse(uint8_t fuse);
		void deactivateFuse(uint8_t fuse);

		void activeAllFuses();
		void deactivateAllFuses();

		uint16_t readFuseCurrent(uint8_t fuse);

		uint16_t readAllFusesCurrent();

		FuseControlerState getSmartFuseState();

	private:
		/*
		 * global state bit
		 * is read at each spi communication
		 */
		uint8_t last_gsb;

		/*
		 * clamping currents
		 */

		uint32_t pin;

		vector<Fuse, 6> fuse;

		GPIO_TypeDef *port;

		SPI_HandleTypeDef *hspi;

		Timer watch_dog;

		FuseControlerState fuse_state;

		/*
		 * helps the management of fuses
		 */
		struct Fuse
		{
			Fuse();

			bool active;

			uint16_t current;

			pair<uint16_t, uint16_t> clamping_currents;
		};
};

template <int num_of_fuse_controlers>
class SmartFuseHandler final
{
	public:
		vector<SmartFuse*, num_of_fuse_controlers> fuses;

		void handle(void);
	private:
};

#endif /* INC_FUSE_H_ */
