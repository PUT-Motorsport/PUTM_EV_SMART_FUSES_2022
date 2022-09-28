/*
 * assi.hpp
 *
 *  Created on: 27 wrz 2022
 *      Author: Piotr Lesicki
 */

#ifndef INC_GPIO_HANDLER_HPP_
#define INC_GPIO_HANDLER_HPP_

#include "stm32l4xx_hal.h"
#include "gpio elements.hpp"
#include "timer.h"
#include "etl/vector.h"

const uint32_t gpio_handler_max_vector_size = 3;

class GpioOutHandler
{
	public:
		void add(GpioOutElement* gpio, uint32_t on_durration, uint32_t off_durration, HandlerToken* token);
		void handle();

	private:
		struct HandlerStruct
		{
			HandlerStruct(GpioOutElement*, uint32_t, uint32_t, HandlerToken*);

			enum struct CurrentState
			{
				Off,
				On
			};

			GpioOutElement* gpio = nullptr;

			Timer timer;

			uint32_t on_durration = 0;
			uint32_t off_durration = 0;

			CurrentState current_state = CurrentState::Off;
			//CurrentState previous_state = CurrentState::Off;

			HandlerToken* token = nullptr;
		};

		etl::vector < HandlerStruct, gpio_handler_max_vector_size > elements;
};

#endif /* INC_GPIO_HANDLER_HPP_ */
