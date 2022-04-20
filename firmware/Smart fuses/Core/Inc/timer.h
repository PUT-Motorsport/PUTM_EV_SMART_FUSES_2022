/*
 * timer.hpp
 *
 *  Created on: 6 kwi 2022
 *      Author: pile
 */

#ifndef INC_TIMER_HPP_
#define INC_TIMER_HPP_

#include "stm32l4xx_hal.h"

class Timer
{
	public:
		Timer();

		void restart();

		uint32_t getPassedTime();

	private:
		uint32_t start_time;
};

#endif /* INC_TIMER_HPP_ */
