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

/* Static functions --------------------------------------------------------------------------------------------------*/

//static std::array < uint8_t, 3 > readRomCommand(uint8_t address)
//{
//	return { 0b11000000 | (address), 0, 0 };
//}
//
//static std::array < uint8_t, 3 > readRamCommand(uint8_t address)
//{
//	return { 0b01000000 | (address), 0, 0 };
//}
//
//static std::array < uint8_t, 3 > readAndClearRamCommand(uint8_t address)
//{
//	return { 0b10000000 | (address), 0, 0 };
//}
//
//static std::array < uint8_t, 3 > writeRamCommand(uint8_t address, uint8_t data1, uint8_t data2)
//{
//	return { 0b00000000 | (address), data1, data2 };
//}
//
//static std::array < uint8_t, 3 > resetCommand()
//{
//	return { 0b11111111, 0, 0 };
//}
//
//static bool ifResetState(std::array < uint8_t, 3 > x)
//{
//	return ((x[0] == 0x00 && x[1] == 0x00 && x[2] == 0x00) || (x[0] & (1 << 6)))
//}

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

SmartFuse::ChannelSettingsAndData::ChannelSettingsAndData()
{
	current = 0x0000;
	state = ChannelState::Ok;
}

/*SmartFuse declarations --------------------------------------------------------------------------------------------*/

SmartFuse::SmartFuse(const GPIO_TypeDef * const port, const uint32_t pin, const SPI_HandleTypeDef * const hspi, std::array < ChannelSettings, number_of_channels_per_fuse > channels_settings) :
					 port(port), pin(pin), hspi(hspi), toggle(false)
{
	for(size_t i = 0; i < number_of_channels_per_fuse; i++)
	{
		this->channels[i].active = channels_settings[i].active;
		this->channels[i].clamping_currents = channels_settings[i].clamping_currents;
		this->channels[i].duty_cycle = channels_settings[i].duty_cycle;
		this->channels[i].latch_off_time_out = channels_settings[i].latch_off_time_out;
		this->channels[i].sampling_mode = channels_settings[i].sampling_mode;

		if(this->channels[i].duty_cycle > 0x3ff) this->channels[i].duty_cycle = 0x3ff;
		if(this->channels[i].latch_off_time_out > 0xf) this->channels[i].latch_off_time_out = 0xf;
	}

	this->action_timer.setTimeOut(100);
	this->watch_dog.setTimeOut(31);

	slaveDeselect();
}

void SmartFuse::setActionInterval(uint32_t interval)
{
	this->action_timer.setTimeOut(interval);
	this->action_timer.restart();
}

void SmartFuse::setAction(etl::delegate<void(SmartFuse*)> action)
{
	this->action = action;
	this->action_defined = true;
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

	this->setUpAllChannelsStates();

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

	/// start timer for watchdog timers
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

	std::array < bool, number_of_channels_per_fuse > read_fuses_states { false };

	std::array < uint8_t, 3 > tx_data { };
	std::array < uint8_t, 3 > rx_data { };

	// reset fuses states
	for(size_t i = 0; i < number_of_channels_per_fuse; i++) this->channels[i].state = ChannelState::Ok;

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
	for(size_t i = 0; i < number_of_channels_per_fuse; i++)
	{
		tx_data[0] = READ_RAM(0x28 + i);
		this->transmitReceiveData(tx_data, rx_data);
		this->channels[i].current = uint16_t(rx_data[1]) << 4 | uint16_t(rx_data[2]) >> 4;
		read_fuses_states[i] = bool(rx_data[2] & (1 << 2));
	}

	//check currents
	for(auto& channel : this->channels)
	{
		if (channel.current < channel.clamping_currents.first)
		{
			channel.active = false;
			lock_state = true;
			fuse_state_changed = true;
			this->state = SmartFuseState::OTPLVDS;
			channel.state = ChannelState::UnderCurrent;
		}
		if (channel.current > channel.clamping_currents.second)
		{
			channel.active = false;
			lock_state = true;
			fuse_state_changed = true;
			this->state = SmartFuseState::OTPLVDS;
			channel.state = ChannelState::OverCurrent;
		}
	}

	if(fuse_state_changed)
	{
		tx_data[0] = WRITE_RAM(0x13);
		tx_data[1] = 0x00;
		for(int i = 0; i < number_of_channels_per_fuse; i++) tx_data[1] |= this->channels[i].active << i;
		tx_data[2] = this->toggle << 1;
		this->transmitReceiveData(tx_data, rx_data);
	}

	//check channel state
	for(size_t i = 0; i < number_of_channels_per_fuse; i++)
	{
		if(this->channels[i].active != read_fuses_states[i])
		{
			this->channels[i].state = ChannelState::STKFLTR;
			lock_state = true;
			this->state = SmartFuseState::OLOFF;
		}
	}

	/// read channel states from sf and handle
	for(size_t i = 0; i < number_of_channels_per_fuse; i++)
	{
		tx_data[0] = READ_AND_CLEAR(0x20 + i);
		this->transmitReceiveData(tx_data, rx_data);
		if(rx_data[1] & (1 << 0)) this->channels[i].state = ChannelState::LatchOff;
		if(rx_data[1] & (1 << 2)) this->channels[i].state = ChannelState::STKFLTR;
		if(rx_data[1] & (1 << 3)) this->channels[i].state = ChannelState::VDSFS;
		if(rx_data[1] & (1 << 4)) this->channels[i].state = ChannelState::CHFBSR;
	}

	if(this->action_defined && this->action_timer.checkIfTimedOutAndReset())
	{
		this->action(this);
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

SmartFuseState SmartFuse::setChannelDutyCykle(Channel channel, uint16_t duty_cykle)
{
	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data { 0, 0, 0 };

	if(duty_cykle > 1023) duty_cykle = 1023;
	this->channels[size_t(channel)].duty_cycle = duty_cykle;

	tx_data[0] = WRITE_RAM(0x00 + size_t(channel));
	tx_data[1] = uint8_t(duty_cykle >> 4);
	tx_data[2] = uint8_t(duty_cykle << 4) | this->toggle << 1;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::activeChannel(Channel channel)
{
	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data { 0, 0, 0 };

	this->channels[size_t(channel)].active = true;

	tx_data[0] = WRITE_RAM(0x13);
	for(int i = 0; i < number_of_channels_per_fuse; i++)
		tx_data[1] |= this->channels[i].active << i;
	tx_data[2] = this->toggle << 1;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateChannel(Channel channel)
{
	std::array < uint8_t, 3 >  tx_data { };
	std::array < uint8_t, 3 >  rx_data;

	this->channels[int(channel)].active = false;

	tx_data[0] = WRITE_RAM(0x13);
	for(int i = 0; i < number_of_channels_per_fuse; i++)
		tx_data[1] |= this->channels[i].active << i;
	tx_data[2] = this->toggle << 1;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::activeAllChannels()
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < 6; i++) this->channels[i].active = true;
	tx_data = { WRITE_RAM(0x13), 0x3f, this->toggle << 1 };
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateAllChannels()
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < number_of_channels_per_fuse; i++)
		this->channels[i].active = false;
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

ChannelState SmartFuse::getChannelState(Channel channel)
{
	return this->channels[size_t(channel)].state;
}

std::array < ChannelState, number_of_channels_per_fuse > SmartFuse::getChannelsStates()
{
	return { this->channels[0].state, this->channels[1].state, this->channels[2].state,
			 this->channels[3].state, this->channels[4].state, this->channels[5].state };
}


std::array < uint16_t, number_of_channels_per_fuse > SmartFuse::getChannelsCurrents()
{
	return { this->channels[0].current, this->channels[1].current, this->channels[2].current,
			 this->channels[3].current, this->channels[4].current, this->channels[5].current };
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
			return;
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
		/// 0x00 to 0x05 - Outputs Control Register - set pwm duty cycles for each channel
		tx_data[0] = WRITE_RAM(0x00 + i);
		tx_data[1] = uint8_t(this->channels[i].duty_cycle >> 4);
		tx_data[2] = uint8_t(this->channels[i].duty_cycle << 4) | this->toggle << 1;
		this->transmitReceiveData(tx_data, rx_data);
	}

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllSamplingModes()
{
	std::array < uint8_t, 3 > tx_data = { 0, 0, 0 };
	std::array < uint8_t, 3 > rx_data;

	/// 0x08 to 0x0d - Outputs Configuration Register - set sampling mode
	for(size_t i = 0; i < number_of_channels_per_fuse; i++)
	{
		tx_data[0] = WRITE_RAM(0x08 + i);
		switch (this->channels[i].sampling_mode)
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
	tx_data[0] = WRITE_RAM(0x10);
	tx_data[1] = this->channels[2].latch_off_time_out << 4 |
			     this->channels[1].latch_off_time_out;
	tx_data[2] = this->channels[0].latch_off_time_out << 4;
	this->transmitReceiveData(tx_data, rx_data);
	tx_data[0] = WRITE_RAM(0x11);
	tx_data[1] = this->channels[5].latch_off_time_out << 4 |
	             this->channels[4].latch_off_time_out;
	tx_data[2] = this->channels[3].latch_off_time_out << 4;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
}

void SmartFuse::setUpAllChannelsStates()
{
	std::array < uint8_t, 3 > tx_data;
	std::array < uint8_t, 3 > rx_data;

	/// 0x13 - Channel Control register - enable channels
	tx_data[0] = WRITE_RAM(0x13);
	tx_data[1] = 0x00;
	for(int i = 0; i < number_of_channels_per_fuse; i++) tx_data[1] |= this->channels[i].active << i;
	tx_data[2] = this->toggle << 1;
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
}

#if SMART_FUSE_CUSTOM_SEND_ENABLE

SmartFuseState SmartFuse::transmitReceiveCustomCommand(std::array < uint8_t, 3 > tx_data, std::array < uint8_t, 3 >& rx_data)
{
	this->transmitReceiveData(tx_data, rx_data);

	this->state = getGSB(rx_data);
	return this->state;
}

#endif

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

uint16_t SmartFuse::getChannelCurrent(Channel channel)
{
	return this->channels[size_t(channel)].current;
}

template <uint32_t num_of_sf>
void SmartFuseHandler<num_of_sf>::emplaceBack(const GPIO_TypeDef * const port, const uint32_t pin, const SPI_HandleTypeDef *const hspi, std::array < ChannelSettings, number_of_channels_per_fuse >channels_settings)
{
	this->smart_fuses.emplace_back(port, pin, hspi, channels_settings);
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
std::array < std::array < ChannelState, number_of_channels_per_fuse >, num_of_sf > SmartFuseHandler<num_of_sf>::getChannelsStates()
{
	std::array < std::array < ChannelState, number_of_channels_per_fuse >, num_of_sf > x;

	for(size_t i = 0; i < num_of_sf; i++)
	{
		x[i] = this->smart_fuses[i].getChannelsStates();
	}

	return x;
}

template <uint32_t num_of_sf>
std::array < std::array < uint16_t, number_of_channels_per_fuse >, num_of_sf > SmartFuseHandler<num_of_sf>::getChannelsCurrents()
{
	std::array < std::array < uint16_t, number_of_channels_per_fuse >, num_of_sf > x;
	for(size_t i = 0; i < num_of_sf; i++)
	{
		x[i] = this->smart_fuses[i].getChannelsCurrents();
	}

	return x;
}

/*template <uint32_t num_of_sf>
const etl::vector < SmartFuse, num_of_sf >& SmartFuseHandler<num_of_sf>::getSmartFuses() const
{
	return this->smart_fuses;
}

template <uint32_t num_of_sf>
const SmartFuse& SmartFuseHandler<num_of_sf>::getSmartFuse(uint8_t index) const
{
	return this->smart_fuses[index];
}*/
