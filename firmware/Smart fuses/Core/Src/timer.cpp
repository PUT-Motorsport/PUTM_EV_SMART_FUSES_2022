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

Timer::Timer(uint32_t timeout) : timeout(timeout)
{
	this->start_time = HAL_GetTick();
}

void Timer::restart()
{
	this->start_time = HAL_GetTick();
}

bool Timer::checkIfTimedOutAndReset()
{
	if(this->getPassedTime() >= this->timeout)
	{
		this->restart();
		return true;
	}

	return false;
}

uint32_t Timer::getPassedTime()
{
	return HAL_GetTick() - this->start_time;
}

uint32_t Timer::getPassedTimeAndReset()
{
	volatile uint32_t passed_time = HAL_GetTick() - this->start_time;
	this->restart();
	return passed_time;
}
