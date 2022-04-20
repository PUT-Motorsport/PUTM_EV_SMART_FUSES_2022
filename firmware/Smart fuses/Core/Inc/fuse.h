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

#define NUMBER


using namespace etl;

enum FuseControlerState
{
	Ok,
	Warning,
	Failed
};

struct Fuse
{
	bool active;

	uint16_t current;
};

class FuseControler
{
	public:
		FuseControler(GPIO_TypeDef *port, uint32_t pin, SPI_HandleTypeDef *hspi);

		void init(void);
		void handle(void);

		FuseControlerState getFuseState();

	private:
		uint32_t pin;

		vector<Fuse, 4> fuse;

		GPIO_TypeDef *port;

		SPI_HandleTypeDef *hspi;

		Timer watch_dog;

		FuseControlerState fuse_state;
};

template <int num_of_fuse_controlers>
class FuseHandler final
{
	public:
		vector<FuseControler, num_of_fuse_controlers> fuses;

		void handle(void);
	private:
};

#endif /* INC_FUSE_H_ */
