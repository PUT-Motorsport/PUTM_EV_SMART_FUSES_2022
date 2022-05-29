/*
 * fuse.cpp
 *
 *  Created on: 7 kwi 2022
 *      Author: Piotr Lesicki
 */

#include "fuse.hpp"
#include "etl/bitset.h"

/*Private defines ---------------------------------------------------------------------------------------------------*/

#define READ_ROM(address)					(0b11000000 | (address))
#define READ_RAM(address)					(0b01000000 | (address))
#define READ_AND_CLEAR(address)				(0b10000000 | (address))
#define WRITE_RAM(address)					(0b00000000 | (address))
#define RESET_SMARTFUSE()					(0b11111111)
#define IF_RESET_STATE(x) 					((x[0] == 0x00 && x[1] == 0x00 && x[2] == 0x00) || (x[0] & (1 << 6)))

/*Private enums -----------------------------------------------------------------------------------------------------*/

/*Static functions --------------------------------------------------------------------------------------------------*/

/// return false if odd parity, returns true if even parity, the pointer should point to a 3 elem tab
static bool checkParity(std::array < uint8_t, 3 > x)
{
	size_t buff = 0;

	std::array< etl::bitset<8>, 3 > bits { x[0], x[1], x[2] };
	for (auto &bit : bits) buff += bit.count();

	return (buff % 2 == 0);
}

/// checks whenever data needs parity bit and adds it or erases it
static void calculateParityBit(std::array < uint8_t, 3 > &data)
{
	//clear parity bit just in case
	data[2] &= ~(1 << 0);
	if (checkParity(data)) data[2] |= (1 << 0);
}

SmartFuseState SmartFuse::getGSB(std::array < uint8_t, 3 > x)
{
	this->last_gsb = x[0];

	if(x[0] & 0b10000000) return SmartFuseState::Ok; 		//at the beginning to save some computing time
	if(x[0] & 0b00000001) return SmartFuseState::FailSafe;
	if(x[0] & 0b00000010) return SmartFuseState::OLOFF;
	if(x[0] & 0b00000100) return SmartFuseState::LatchOff;
	if(x[0] & 0b00001000) return SmartFuseState::TempFail;
	if(x[0] & 0b00010000) return SmartFuseState::OTPLVDS;
	if(x[0] & 0b00100000) return SmartFuseState::SPIError;
	if(x[0] & 0b01000000) return SmartFuseState::ResetState;
	return SmartFuseState::NotResponding;
}

/*Fuse declarations -------------------------------------------------------------------------------------------------*/

SmartFuse::Fuse::Fuse()
{
	active = false;
	current = 0x0000;
	state = FuseState::Ok;
}

/*SmartFuse declarations --------------------------------------------------------------------------------------------*/

SmartFuse::SmartFuse(const GPIO_TypeDef * const port, const uint32_t pin, const SPI_HandleTypeDef * const hspi, const FusesSettings &fuses_settings) :
					 port(port), pin(pin), hspi(hspi), fuses_settings(fuses_settings), toggle(false)
{
	for (int i = 0; i < 6; i++)
	{
		this->fuses[i].clamping_currents = fuses_settings.clamping_currents[i];
		this->fuses[i].active = fuses_settings.active[i];
	}

	slaveDeselect();
}

void SmartFuse::slaveSelect(void)
{
	HAL_GPIO_WritePin((GPIO_TypeDef*)(this->port), this->pin, GPIO_PIN_RESET);
}

void SmartFuse::slaveDeselect(void)
{
	HAL_GPIO_WritePin((GPIO_TypeDef*)(this->port), this->pin, GPIO_PIN_SET);
}

SmartFuseState SmartFuse::init(void)
{
	this->reset();

	this->setUpAllDutyCycles();

	this->setUpAllSamplingModes();

	this->setUpAllLatchOffTimers();

	this->setUpAllChanelsStates();

	return this->state;
}

SmartFuseState SmartFuse::enable()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	/// set unlock bit in ctrl reg
	tx_data = { WRITE_RAM(0x14), 1 << 6, 0 };
	this->transmitReceiveData(tx_data, rx_data);

	/// set enable bit in ctrl reg
	tx_data = { WRITE_RAM(0x14), 1 << 3, 0 };
	this->transmitReceiveData(tx_data, rx_data);

	/// start timer for watchdog timer
	this->watch_dog.restart();
	this->toggle = false;

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::disable()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	/// set unlock bit in ctrl reg
	tx_data = { WRITE_RAM(0x14), 1 << 6, 0 };
	this->transmitReceiveData(tx_data, rx_data);

	/// reset enable bit in ctrl reg
	tx_data = { WRITE_RAM(0x14), 0, 0 };
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::handle(void)
{
	bool lock_state = false;
	bool fuse_state_changed = false;

	std::array < bool, 6 > read_fuses_states { false };

	std::array < uint8_t, 3 > tx_data { 0 };
	std::array < uint8_t, 3 > rx_data { 0 };

	// reset fuses states
	for(size_t i = 0; i < 6; i++) this->fuses[i].state = FuseState::Ok;

	//handle timer
	if(watch_dog.getPassedTime() >= 31)
	{
		this->toggle = !this->toggle;
		tx_data[0] = READ_RAM(0x13);
		this->transmitReceiveData(tx_data, rx_data);
		rx_data[2] &= ~(1 << 1);
		tx_data = { WRITE_RAM(0x13), rx_data[1], rx_data[2] |= (toggle << 1) };
		this->transmitReceiveData(tx_data, rx_data);
		this->watch_dog.restart();
	}

	//read currents
	for(size_t i = 0; i < 6; i++)
	{
		tx_data[0] = READ_RAM(0x28 + i);
		this->transmitReceiveData(tx_data, rx_data);
		this->fuses[i].current = uint16_t(rx_data[1]) << 4 | uint16_t(rx_data[2]) >> 4;
		read_fuses_states[i] = bool(rx_data[2] & (1 << 2));
	}

	//check currents
	for(size_t i = 0; i < 6; i++)
	{
		if (this->fuses[i].current < this->fuses[i].clamping_currents.first)
		{
			this->fuses[i].active = false;
			lock_state = true;
			fuse_state_changed = true;
			this->state = SmartFuseState::OTPLVDS;
			this->fuses[i].state = FuseState::UnderCurrent;
		}
		if (this->fuses[i].current > this->fuses[i].clamping_currents.second)
		{
			this->fuses[i].active = false;
			lock_state = true;
			fuse_state_changed = true;
			this->state = SmartFuseState::OTPLVDS;
			this->fuses[i].state = FuseState::OverCurrent;
		}
	}

	if(fuse_state_changed)
	{
		tx_data[0] = WRITE_RAM(0x13);
		tx_data[1] = 0x00;
		for(int i = 0; i < 6; i++) tx_data[1] |= this->fuses[i].active << i;
		tx_data[2] = this->toggle << 1;
		this->transmitReceiveData(tx_data, rx_data);
	}

	//check chanel state
	for(size_t i = 0; i < 6; i++)
	{
		if(this->fuses[i].active != read_fuses_states[i])
		{
			this->fuses[i].state = FuseState::ShortedToGround;
			lock_state = true;
			this->state = SmartFuseState::OLOFF;
		}
	}

	/// read chanel states from sf and handle
	for(size_t i = 0; i < 6; i++)
	{
		tx_data[0] = READ_AND_CLEAR(0x20 + i);
		this->transmitReceiveData(tx_data, rx_data);
		if(rx_data[1] & (1 << 0)) this->fuses[i].state = FuseState::LatchOff;
		if(rx_data[1] & (1 << 2)) this->fuses[i].state = FuseState::STKFLTR;
		if(rx_data[1] & (1 << 3)) this->fuses[i].state = FuseState::VDSFS;
		if(rx_data[1] & (1 << 4)) this->fuses[i].state = FuseState::CHFBSR;
	}

	if (!lock_state) this->state = getGSB(rx_data);
	return this->state;
}

/// handle timer only
SmartFuseState SmartFuse::handleTimer(void)
{
	if(watch_dog.getPassedTime() >= 31)
	{
		std::array < uint8_t, 3 > tx_data { 0, 0, 0 };
		std::array < uint8_t, 3 > rx_data { 0, 0, 0 };

		this->toggle = !this->toggle;
		tx_data[0] = READ_RAM(0x13);
		this->transmitReceiveData(tx_data, rx_data);
		rx_data[2] &= ~(1 << 1);
		tx_data = { WRITE_RAM(0x13), rx_data[1], rx_data[2] |= (toggle << 1) };
		this->transmitReceiveData(tx_data, rx_data);
		this->watch_dog.restart();

		this->state = getGSB(rx_data);
	}

	return this->state;
}

SmartFuseState SmartFuse::setFuseDutyCykle(FuseNumber fuse, uint16_t duty_cykle)
{
	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data { 0, 0, 0 };

	tx_data = { WRITE_RAM(0x00 + int(fuse)), uint8_t(duty_cykle >> 8), uint8_t(duty_cykle << 4) | this->toggle << 1 };
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::activeFuse(FuseNumber fuse)
{
	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data { 0, 0, 0 };

	this->fuses[int(fuse)].active = true;

	for(int i = 0; i < 6; i++)
		tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateFuse(FuseNumber fuse)
{
	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data;

	this->fuses[int(fuse)].active = false;

	for(int i = 0; i < 6; i++)
		tx_data[0] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::activeAllFuses(void)
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < 6; i++) this->fuses[i].active = true;
	tx_data = { WRITE_RAM(0x13), 0x3f, this->toggle << 1 };
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateAllFuses()
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < 6; i++)
		this->fuses[i].active = false;
	tx_data = { WRITE_RAM(0x13), 0x00, this->toggle << 1 };
	this->transmitReceiveData(tx_data, rx_data);

//	modifyTab(tx_data, READ_RAM(0x13), 0x00, 0x00);
//	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::getState() const
{
	return this->state;
}

std::array < FuseState, 6 > SmartFuse::getFuseStates()
{
	return { this->fuses[0].state, this->fuses[1].state, this->fuses[2].state,
			 this->fuses[3].state, this->fuses[4].state, this->fuses[5].state };
}

uint8_t SmartFuse::getLastGSB() const
{
	return this->last_gsb;
}

void SmartFuse::reset()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	/// reset smart fuse
	tx_data = { RESET_SMARTFUSE(), 0, 0 };
	this->transmitReceiveData(tx_data, rx_data);

	/// wait for smart fuse to wake up
	tx_data = { READ_ROM(0x01), 0, 0 };
	for (size_t i = 0; i < fuse_timeout; i++)
	{
		if(!IF_RESET_STATE(rx_data)) break;
		else if(i == fuse_timeout - 1)
		{
			this->state = SmartFuseState::NotResponding;
		}
		this->transmitReceiveData(tx_data, rx_data);
		HAL_Delay(1);
	}

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllDutyCycles()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	for(size_t i = 0; i < 6; i++)
	{
		/// 0x00 to 0x05 - Outputs Control Register - set pwm duty cycles for each chanel
		tx_data[0] = WRITE_RAM(0x00 + i);
		tx_data[1] = uint8_t(this->fuses_settings.duty_cycle[i] >> 4);
		tx_data[2] = uint8_t(this->fuses_settings.duty_cycle[i] << 4) | this->toggle << 1;
		this->transmitReceiveData(tx_data, rx_data);
	}

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllSamplingModes()
{
	std::array < uint8_t, 3 > tx_data = { 0, 0, 0 };
	std::array < uint8_t, 3 > rx_data;

	/// 0x08 to 0x0d - Outputs Configuration Register - set sampling mode
	for(size_t i = 0; i < 6; i++)
	{
		tx_data[0] = WRITE_RAM(0x08 + i);
		switch (this->fuses_settings.sampling_mode[i])
		{
			case SamplingMode::Stop: tx_data[2] = 0x00; break;
			case SamplingMode::Start: tx_data[2] = 0x40; break;
			case SamplingMode::Continuous: tx_data[2] = 0x80; break;
			case SamplingMode::Filtered: tx_data[2] = 0xc0; break;
		}
		this->transmitReceiveData(tx_data, rx_data);
	}

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllLatchOffTimers()
{
	std::array < uint8_t, 3 > tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 > rx_data { 0, 0, 0 };

	/// 0x10 to 0x11 - Channel Latch OFF Timer Control register - set latch off
	tx_data[1] = this->fuses_settings.latch_off_time_out[2] << 4 |
		   this->fuses_settings.latch_off_time_out[1];
	tx_data[2] = this->fuses_settings.latch_off_time_out[1] << 4;
	tx_data[0] = WRITE_RAM(0x10);
	this->transmitReceiveData(tx_data, rx_data);
	tx_data[1] = this->fuses_settings.latch_off_time_out[5] << 4 |
	       this->fuses_settings.latch_off_time_out[4];
	tx_data[2] = this->fuses_settings.latch_off_time_out[3] << 4;
	tx_data[0] = WRITE_RAM(0x11);
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllChanelsStates()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	/// 0x13 - Channel Control register - enable channels
	tx_data[0] = WRITE_RAM(0x13);
	tx_data[1] = 0x00;
	for(int i = 0; i < 6; i++) tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
}

void SmartFuse::transmitReceiveData(std::array<uint8_t, 3> tx_data, std::array<uint8_t, 3> &rx_data)
{
	/// just check
	if (((&hspi1)->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE) __HAL_SPI_ENABLE(&hspi1);

	/// data needs to have a parity check bit
	calculateParityBit(tx_data);

	this->slaveSelect();

	for (uint8_t tx = 0, rx = 0; tx < 3 || rx < 3;)
	{
		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) && rx < 3)
		{
			(*(uint8_t*) &rx_data[rx]) = *(__IO uint8_t *) &(&hspi1)->Instance->DR;
			rx++;
		}

		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) && tx < 3)
		{
			*(__IO uint8_t *) &(&hspi1)->Instance->DR = tx_data[tx];
			tx++;
		}
	}

	this->slaveDeselect();
}

uint16_t SmartFuse::getFuseCurrent(FuseNumber fuse)
{
	return this->fuses[size_t(fuse)].current;
}

template <uint32_t num_of_sf>
SmartFuseState SmartFuseHandler<num_of_sf>::handleAll()
{
	SmartFuseState result = SmartFuseState::Ok;

	for(auto &smart_fuse : smart_fuses)
	{
		SmartFuseState x = smart_fuse.handle();
		if(x != SmartFuseState::Ok) result = x;
	}

	return result;
}

template <uint32_t num_of_sf>
SmartFuseState SmartFuseHandler<num_of_sf>::initAll()
{
	SmartFuseState result = SmartFuseState::Ok;

	for(auto &smart_fuse : smart_fuses)
	{
		SmartFuseState x = smart_fuse.init();
		if(x != SmartFuseState::Ok) result = x;
	}

	return result;
}

template <uint32_t num_of_sf>
SmartFuseState SmartFuseHandler<num_of_sf>::enableAll()
{
	SmartFuseState result = SmartFuseState::Ok;

	for(auto &smart_fuse : smart_fuses)
	{
		SmartFuseState x = smart_fuse.enable();
		HAL_Delay(5);
		if(x != SmartFuseState::Ok) result = x;
	}

	return result;
}

template <uint32_t num_of_sf>
SmartFuseState SmartFuseHandler<num_of_sf>::disableAll()
{
	SmartFuseState result = SmartFuseState::Ok;

	for(auto &smart_fuse : smart_fuses)
	{
		SmartFuseState x = smart_fuse.disable();
		if(x != SmartFuseState::Ok) result = x;
	}

	return result;
}

template <uint32_t num_of_sf>
std::array < SmartFuseState, num_of_sf >  SmartFuseHandler<num_of_sf>::getStates()
{
	std::array < SmartFuseState, num_of_sf > x;

	for(size_t i = 0; i < num_of_sf; i++)
	{
		x[i] = this->smart_fuses[i].getState();
	}

	return x;
}

template <uint32_t num_of_sf>
std::array < std::array < FuseState, 6 >, num_of_sf > SmartFuseHandler<num_of_sf>::getChanelsStates()
{
	std::array < std::array < FuseState, 6 >, num_of_sf > x;

	for(size_t i = 0; i < num_of_sf; i++)
	{
		x[i] = this->smart_fuses[i].getFuseStates();
	}

	return x;
}
