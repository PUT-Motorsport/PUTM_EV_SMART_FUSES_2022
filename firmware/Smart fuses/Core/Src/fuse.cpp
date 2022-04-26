/*
 * fuse.cpp
 *
 *  Created on: 7 kwi 2022
 *      Author: Piotr Lesicki
 */

#include "fuse.h"

/*Private defines ---------------------------------------------------------------------------------------------------*/

#define READ_ROM(address)					(0b11000000 | (address))
#define READ_RAM(address)					(0b01000000 | (address))
#define READ_AND_CLEAR(address)				(0b10000000 | (address))
#define WRITE_RAM(address)					(0b00000000 | (address))
#define RESET_SMARTFUSE()					(0b11111111)
#define WAIT_FOR_VALID_RESPONSE(x) 			((x[0] == 0x00 && x[1] == 0x00 && x[2] == 0x00) || (x[0] & (1 << 6)))
#define MODIFY(array, x0, x1, x2) 			array[0] = x0; array[1] = x1; array[2] = x2;

/*Static functions --------------------------------------------------------------------------------------------------*/

static void calculateParityBit(uint8_t* x)
{
	///some memory hacking don't do this at home kids
	uint32_t buff = *(uint32_t*)(x);

	/// clear unwanted trash and parity bit
	buff &= ~(0xff << 24 | 1 << 0);

	/// some math wizardry to quickly calculate if parity bit is needed
	buff ^= (buff >> 16);
	buff ^= (buff >> 8);
	buff ^= (buff >> 4);
	buff ^= (buff >> 2);
	buff ^= (buff >> 1);

	if (buff & 0x01) *(x + 2) |= (1 << 0);
}

static SmartFuseState checkGSB(uint8_t gsb)
{
	if(gsb & 0b10000000) return SmartFuseState::Ok; 		//at the beginning to save some computing time
	if(gsb & 0b00000001) return SmartFuseState::FailSafe;
	if(gsb & 0b00000010) return SmartFuseState::OLOFF;
	if(gsb & 0b00000100) return SmartFuseState::LatchOff;
	if(gsb & 0b00001000) return SmartFuseState::TempFail;
	if(gsb & 0b00010000) return SmartFuseState::OTPLVDS;
	if(gsb & 0b00100000) return SmartFuseState::SPIError;
	if(gsb & 0b01000000) return SmartFuseState::ResetState;
	return SmartFuseState::NotResponding;
}

/*Fuse declarations -------------------------------------------------------------------------------------------------*/

Fuse::Fuse()
{
	active = false;
	current = 0x0000;
}

/*SmartFuse declarations --------------------------------------------------------------------------------------------*/

SmartFuse::SmartFuse(const GPIO_TypeDef *port, const uint32_t pin, const SPI_HandleTypeDef *hspi, const FusesSettings *fuses_settings) :
					 port(port), pin(pin), hspi(hspi), fuses_settings(fuses_settings)
{
	this->port = port;
	this->pin = pin;
	this->hspi = hspi;
	this->fuses_settings = fuses_settings;
	this->toggle = false;
	this->fuse_state_changed = false;

	for (int i = 0; i < 6; i++)
	{
		this->fuses[i].clamping_currents = fuses_settings->clamping_currents[i];
		this->fuses[i].active = fuses_settings->active[i];
	}

	slaveDeselect();
}

void SmartFuse::slaveSelect(void)
{
	HAL_GPIO_WritePin(this->port, this-pin, RESET);
}

void SmartFuse::slaveDeselect(void)
{
	HAL_GPIO_WritePin(this->port, this-pin, SET);
}

SmartFuseState SmartFuse::init(void)
{
	uint8_t tx_data[3] = { 0, 0, 0 };
	uint8_t rx_data[3] = { 0, 0, 0 };

	/// reset smart fuse
	tx_data[0] = RESET_SAMRTFUSE();
	transmitReceiveData(tx_data, rx_data);

	/// wait for smart fuse to wake up
	tx_data[0] = READ_ROM(0x00);
	for (int i = 0; i < FUSE_TIMEOUT; i++)
	{
		if(!IF_RESET_STATE(rx_data)) break;
		else if(i == 5)
		{
			this->state = SmartFuseState::NotResponding;
			goto end;
		}
		transmitReceiveData(tx_data, rx_data);
	}

	/// set unlock bit in ctrl reg
	MODIFY(tx_data, WRITE_RAM(0x14), 1 << 6, 0);
	transmitReceiveData(tx_data, rx_data);

	/// set enable bit in ctrl reg
	MODIFY(tx_data, WRITE_RAM(0x14), 1 << 3, 0);
	transmitReceiveData(tx_data, rx_data);

	/// start watch dog timer management
	this->watch_dog.restart();
	this->toggle = false;

	for(int i = 0; i < 6; i++)
	{
		/// 0x00 to 0x05 - Outputs Control Register - set pwm duty cycles for each chanel
		tx_data[1] = uint8_t(this->fuses_settings.duty_cykle[i] >> 8);
		tx_data[2] = uint8_t(this->fuses_settings.duty_cykle[i] << 4) | this->toggle << 1;
		tx_data[0] = WRITE_RAM(0x00 + i);
		transmitReceiveData(tx_data, rx_data);

		/// 0x08 to 0x0d - Outputs Configuration Register - set sampling mode
		tx_data[1] = 0x00;
		switch (this->fuses_settings.sampling_mode[i])
		{
			case SamplingMode::Stop: tx_data[2] = 0x00; break;
			case SamplingMode::Start: tx_data[2] = 0x40; break;
			case SamplingMode::Continuous: tx_data[2] = 0x80; break;
			case SamplingMode::Filtered: tx_data[2] = 0xc0; break;
		}
		tx_data = WRITE_RAM(0x08 + i);
		transmitReceiveData(tx_data, rx_data);

		/// just in case
		if(watch_dog.getPassedTime() >= 30)
		{
			tx_data[0] = READ_RAM(0x13);
			transmitReceiveData(tx_data, rx_data);
			MODIFY(tx_data, WRITE_RAM(0x13), rx_data[1], rx_data[2] ^= (1 << 1));
			transmitReceiveData(tx_data, rx_data);
			fuse_watch_dog.restart();
			this->toggle = !this->toggle;
		}
	}

	/// 0x10 to 0x11 - Channel Latch OFF Timer Control register - set latch off
	tx_data[1] = this->fuses_settings.latch_off_time_out[2] << 4 |
		   this->fuses_settings.latch_off_time_out[1];
	tx_data[2] = this->fuses_settings.latch_off_time_out[1] << 4;
	tx_data[0] = WRITE_RAM(0x10);
	transmitReceiveData(tx_data, rx_data);
	tx_data[1] = this->fuses_settings.latch_off_time_out[5] << 4 |
	       this->fuses_settings.latch_off_time_out[4];
	tx_data[2] = this->fuses_settings.latch_off_time_out[3] << 4;
	tx_data[0] = WRITE_RAM(0x11);
	transmitReceiveData(tx_data, rx_data);

	/// 0x13 - Channel Control register - enable channels
	tx_data[1] = 0x00;
	for(int i = 0; i < 6; i++)
		tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	transmitReceiveData(tx_data, rx_data);

end:
	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::handle(void)
{
	uint8_t tx_data[3] = { 0, 0, 0 };
	uint8_t rx_data[3] = { 0, 0, 0 };

	if(watch_dog.getPassedTime() >= 30)
	{
		tx_data[0] = READ_RAM(0x13);
		transmitReceiveData(tx_data, rx_data);
		MODIFY(tx_data, WRITE_RAM(0x13), rx_data[1], rx_data[2] ^= (1 << 1));
		transmitReceiveData(tx_data, rx_data);
		fuse_watch_dog.restart();
		this->toggle = !this->toggle;
	}

	for(int i = 0; i , 6; i++)
	{
		tx_data[0] = READ_RAM(0x28 + i);
		transmitReceiveData(tx_data, rx_data);
		this->fuses[i].current = (*(uint16_t*)(rx_data + 1) >> 4) & 0x03ff;
	}

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::setFuseDutyCykle(FuseNumber fuse, uint16_t duty_cykle)
{
	uint8_t hbit = uint8_t(this->fuse_settings->duty_cykle[i] >> 8);
	uint8_t lbit = uint8_t(this->fuse_settings->duty_cykle[i] << 4) | this->toggle << 1;
	MODIFY(tx_data, WRITE_RAM(0x00 + fuse), hbit, lbit);
	transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::activeFuse(FuseNumber fuse)
{
	uint8_t tx_data[3] = { 0, 0, 0 };
	uint8_t rx_data[3] = { 0, 0, 0 };

	this->fuses[fuse].active = true;

	for(int i = 0; i < 6; i++)
		tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateFuse(FuseNumber fuse)
{
	uint8_t tx_data[3] = { 0, 0, 0 };
	uint8_t rx_data[3];

	this->fuses[fuse].active = false;

	for(int i = 0; i < 6; i++)
		tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::activeAllFuses(void)
{
	uint8_t tx_data[3];
	uint8_t rx_data[3];

	for(int i = 0; i < 6; i++)
		this->fuses[i].active = true;
	tx_data[1] |= 0x3f;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::deactivateAllFuses(void)
{
	uint8_t tx_data[3];
	uint8_t rx_data[3];

	for(int i = 0; i < 6; i++)
		this->fuses[i].active = false;
	tx_data[1] = 0x00;
	tx_data[2] = this->toggle << 1;
	tx_data[0] = WRITE_RAM(0x13);
	transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data);
	return this->state;
}

SmartFuseState SmartFuse::getSmartFuseState(void)
{
	return this->state;
}

void SmartFuse::transmitReceiveData(uint8_t *tx_data, uint8_t *rx_data)
{
	/// just check
	if (((&hspi1)->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE) __HAL_SPI_ENABLE(&hspi1);

	/// data needs to have a parity check bit
	calculateParityBit(tx_data);

	slaveSelect();

	for (uint8_t tx = 0, rx = 0; tx < 3 || rx < 3;)
	{
		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) && rx < 3)
		{
			(*(uint8_t*) rx_data) = *(__IO uint8_t *) &(&hspi1)->Instance->DR;
			rx_data++;
			rx++;
		}

		if (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) && tx < 3)
		{
			*(__IO uint8_t *) &(&hspi1)->Instance->DR = (*tx_data);
			tx_data++;
			tx++;
		}
	}

	slaveDeselect();
}

template <int num_of_sf>
bool SmartFuseHandler<num_of_sf>::handle(void)
{
	bool result;

	for(auto smart_fuse = this->smart_fuses.begin(); fuse != this->smart_fuses.end(); smart_fuse++)
		if(smart_fuse->handle() != SmartFuseState::Ok) result = false;
}
