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
		Timer(uint32_t);

		void restart();
		void setTimeOut(uint32_t);

		bool checkIfTimedOutAndReset();

		uint32_t getPassedTime();
		uint32_t getPassedTimeAndReset();

	private:
		volatile uint32_t start_time;
		volatile uint32_t timeout = 0;
};

#endif /* INC_TIMER_HPP_ */
