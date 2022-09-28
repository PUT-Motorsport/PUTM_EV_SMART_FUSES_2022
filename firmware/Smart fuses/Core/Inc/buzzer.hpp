/*
 * pwm.hpp
 *
 *  Created on: 27 wrz 2022
 *      Author: pile
 */

#ifndef INC_BUZZER_HPP_
#define INC_BUZZER_HPP_

#include "timer.h"
#include "tokens.hpp"
#include "etl/vector.h"
#include "gpio elements.hpp"

class Buzzer : public GpioOutElement
{
	public:
		Buzzer(const GPIO_TypeDef * const port, const uint32_t pin, const bool is_inverted, uint32_t active_period);

		virtual void handle() override;
	private:
		Timer timer;

		uint32_t active_period = 0;

		bool started = false;
};

#endif /* INC_BUZZER_HPP_ */
