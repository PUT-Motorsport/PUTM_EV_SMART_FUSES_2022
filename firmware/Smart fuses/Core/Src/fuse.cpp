/*
 * fuse.cpp
 *
 *  Created on: 7 kwi 2022
 *      Author: pile
 */

#include "fuse.h"

Fuse::Fuse()
{
	active = false;
	current = 0x0000;
}

SmartFuse::SmartFuse(GPIO_TypeDef *port, uint32_t pin, SPI_HandleTypeDef *hspi)
{

}

void SmartFuse::init(void)
{

}

void SmartFuse::handle(void)
{

}

void SmartFuse::setFuseDutyCykle(uint8_t fuse, uint16_t duty_cykle)
{

}

void SmartFuse::activeFuse(uint8_t fuse)
{

}

void SmartFuse::deactivateFuse(uint8_t fuse)
{

}

void SmartFuse::activeAllFuses()
{

}

void SmartFuse::deactivateAllFuses()
{

}

uint16_t SmartFuse::readFuseCurrent(uint8_t fuse)
{

}

uint16_t SmartFuse::readAllFusesCurrent();

FuseControlerState SmartFuse::getFuseState();
