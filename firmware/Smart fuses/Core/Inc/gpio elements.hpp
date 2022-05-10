/*
 * gpio elements.hpp
 *
 *  Created on: 6 maj 2022
 *      Author: Piotr Lesicki
 */

#ifndef INC_GPIO_ELEMENTS_HPP_
#define INC_GPIO_ELEMENTS_HPP_

#include "stm32l4xx_hal.h"

class GpioElement
{
	public:
		GpioElement(const GPIO_TypeDef * const port, const uint32_t pin, const bool is_inverted);

		//virtual void handle();
		virtual ~GpioElement() { };

	protected:
		bool state;
		bool is_inverted;

		const uint32_t pin;

		const GPIO_TypeDef * const port;
};

class GpioOutElement : public GpioElement
{
	public:
		GpioOutElement(const GPIO_TypeDef * const port, const uint32_t pin);
		GpioOutElement(const GPIO_TypeDef * const port, const uint32_t pin, const bool is_inverted);

		void activate();
		void deactivate();

		//void handle() override;
		//void toogle();
};

class GpioInElement : public GpioElement
{
	public:
		GpioInElement(const GPIO_TypeDef * const port, const uint32_t pin);
		GpioInElement(const GPIO_TypeDef * const port, const uint32_t pin, const bool is_inverted);

		//void handle() override;

		bool isActive();
};

#endif /* INC_GPIO_ELEMENTS_HPP_ */
