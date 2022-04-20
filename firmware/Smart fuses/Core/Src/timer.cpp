/*
 * timer.cpp
 *
 *  Created on: 6 kwi 2022
 *      Author: pile
 */

#include "timer.h"

Timer::Timer()
{
	this->start_time = HAL_GetTick();
}

uint32_t Timer::getPassedTime()
{
	return HAL_GetTick() - this->start_time;
}

void Timer::restart()
{
	this->start_time = HAL_GetTick();
}

