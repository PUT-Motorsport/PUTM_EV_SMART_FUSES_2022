/*
 * pwm.cpp
 *
 *  Created on: 27 wrz 2022
 *      Author: pile
 */

#include "buzzer.hpp"

Buzzer::Buzzer(const GPIO_TypeDef * const port, const uint32_t pin, const bool is_inverted, uint32_t active_period) : GpioOutElement(port, pin, is_inverted), active_period(active_period) { }

void Buzzer::handle()
{
	if (token_source.enabled() && !started)
	{
		timer.setTimeOut(active_period);
		timer.restart();
		started = true;
	}
	if (!token_source.enabled()) started = false;

	if (!started) return;
	if (!timer.checkIfTimedOutAndReset()) return;

	token_source.stop();
}
