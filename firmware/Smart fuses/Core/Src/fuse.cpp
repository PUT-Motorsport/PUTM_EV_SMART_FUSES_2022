/*
 * fuse.cpp
 *
 *  Created on: 7 kwi 2022
 *      Author: Piotr Lesicki
 */

#include <fuse.hpp>

/*Private defines ---------------------------------------------------------------------------------------------------*/

#define READ_ROM(address)					(0b11000000 | (address))
#define READ_RAM(address)					(0b01000000 | (address))
#define READ_AND_CLEAR(address)				(0b10000000 | (address))
#define WRITE_RAM(address)					(0b00000000 | (address))
#define RESET_SMARTFUSE()					(0b11111111)
#define IF_RESET_STATE(x) 			((x[0] == 0x00 && x[1] == 0x00 && x[2] == 0x00) || (x[0] & (1 << 6)))

#define FOR_SLOWPOKES true

/*Static functions --------------------------------------------------------------------------------------------------*/

/// return false if odd parity, returns true if even parity, the pointer should point to a 3 elem tab
static bool checkParity(std::array<uint8_t, 3> x)
{
	uint8_t buff = 0;
	for(int i = 0; i < 3; i++) for(int j = 0; j < 8; j++) if(x[i] & 1 << j) buff++;
	return (buff % 2 == 0 ? true : false);
}

/// checks whenever data needs parity bit and adds it or erases it
static void calculateParityBit(std::array<uint8_t, 3> &data)
{
	//clear parity bit just in case
	data[2] &= ~(1 << 0);
	if (checkParity(data)) data[2] |= (1 << 0);
}

static void modifyTab(std::array<uint8_t, 3> &tab, uint8_t dat_1, uint8_t dat_2, uint8_t dat_3)
{
	tab[0] = dat_1;
	tab[1] = dat_2;
	tab[2] = dat_3;
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

SmartFuse::Fuse::Fuse()
{
	active = false;
	current = 0x0000;
}

/*SmartFuse declarations --------------------------------------------------------------------------------------------*/

SmartFuse::SmartFuse(const GPIO_TypeDef * const port, const uint32_t pin, const SPI_HandleTypeDef * const hspi, const FusesSettings &fuses_settings) :
					 port(port), pin(pin), hspi(hspi), fuses_settings(fuses_settings)
{
	this->toggle = false;

	for (int i = 0; i < 6; i++)
	{
		this->fuses[i].clamping_currents = fuses_settings.clamping_currents[i];
		this->fuses[i].active = fuses_settings.active[i];
	}

	slaveDeselect();
}

void SmartFuse::slaveSelect(void)
{
	HAL_GPIO_WritePin( (GPIO_TypeDef*)(this->port), this->pin, GPIO_PIN_RESET);
}

void SmartFuse::slaveDeselect(void)
{
	HAL_GPIO_WritePin((GPIO_TypeDef*)(this->port), this->pin, GPIO_PIN_SET);
}

SmartFuseState SmartFuse::init(void)
{
	std::array<uint8_t, 3> tx_data { 0, 0, 0 };
	std::array<uint8_t, 3> rx_data { 0, 0, 0 };

	//modifyTab(tx_data, 0, 0, 0);

	/// reset smart fuse
	modifyTab(tx_data, RESET_SMARTFUSE(), 0, 0);
	transmitReceiveData(tx_data, rx_data);

	/// wait for smart fuse to wake up

	modifyTab(tx_data, READ_ROM(0x01), 0, 0);
	for (int i = 0; i < FUSE_TIMEOUT; i++)
	{
		if(!IF_RESET_STATE(rx_data)) break;
		else if(i == FUSE_TIMEOUT - 1)
		{
			this->state = SmartFuseState::NotResponding;
			goto end;
		}
		transmitReceiveData(tx_data, rx_data);
		HAL_Delay(1);
	}

	/// set unlock bit in ctrl reg
	modifyTab(tx_data, WRITE_RAM(0x14), 1 << 6, 0);
	transmitReceiveData(tx_data, rx_data);

	/// set enable bit in ctrl reg
	modifyTab(tx_data, WRITE_RAM(0x14), 1 << 3, 0);
	transmitReceiveData(tx_data, rx_data);

	/// start watch dog timer management
	this->watch_dog.restart();
	this->toggle = false;

	for(int i = 0; i < 6; i++)
	{
		/// 0x00 to 0x05 - Outputs Control Register - set pwm duty cycles for each chanel
		tx_data[0] = WRITE_RAM(0x00 + i);
		tx_data[1] = uint8_t(this->fuses_settings.duty_cykle[i] >> 4);
		tx_data[2] = uint8_t(this->fuses_settings.duty_cykle[i] << 4) | this->toggle << 1;
		transmitReceiveData(tx_data, rx_data);

		/// 0x08 to 0x0d - Outputs Configuration Register - set sampling mode
		tx_data[0] = WRITE_RAM(0x08 + i);
		tx_data[1] = 0x00;
		switch (this->fuses_settings.sampling_mode[i])
		{
			case SamplingMode::Stop: tx_data[2] = 0x00; break;
			case SamplingMode::Start: tx_data[2] = 0x40; break;
			case SamplingMode::Continuous: tx_data[2] = 0x80; break;
			case SamplingMode::Filtered: tx_data[2] = 0xc0; break;
		}
		transmitReceiveData(tx_data, rx_data);

		/// just in case
		this->handle_timer();
	}

	/// 0x10 to 0x11 - Channel Latch OFF Timer Control register - set latch off
//	tx_data[1] = this->fuses_settings.latch_off_time_out[2] << 4 |
//		   this->fuses_settings.latch_off_time_out[1];
//	tx_data[2] = this->fuses_settings.latch_off_time_out[1] << 4;
//	tx_data[0] = WRITE_RAM(0x10);
//	transmitReceiveData(tx_data, rx_data);
//	tx_data[1] = this->fuses_settings.latch_off_time_out[5] << 4 |
//	       this->fuses_settings.latch_off_time_out[4];
//	tx_data[2] = this->fuses_settings.latch_off_time_out[3] << 4;
//	tx_data[0] = WRITE_RAM(0x11);
//	transmitReceiveData(tx_data, rx_data);

	/// 0x13 - Channel Control register - enable channels
	tx_data[0] = WRITE_RAM(0x13);
	tx_data[1] = 0x00;
	for(int i = 0; i < 6; i++) tx_data[1] |= this->fuses[i].active << i;
	tx_data[2] = this->toggle << 1;
	transmitReceiveData(tx_data, rx_data);

end:
	this->state = checkGSB(rx_data[0]);
	return this->state;
}

//handle everything
SmartFuseState SmartFuse::handle(void)
{
	std::array < uint8_t, 3 > tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 > rx_data { 0, 0, 0 };

	if(watch_dog.getPassedTime() >= 40)
	{
		this->toggle = !this->toggle;
		tx_data[0] = READ_RAM(0x13);
		transmitReceiveData(tx_data, rx_data);
		rx_data[2] &= ~(1 << 1);
		modifyTab(tx_data, WRITE_RAM(0x13), rx_data[1], rx_data[2] |= (toggle << 1));
		transmitReceiveData(tx_data, rx_data);
		this->watch_dog.restart();
	}

	for(int i = 0; i < 6; i++)
	{
		tx_data[0] = READ_RAM(0x28 + i);
		transmitReceiveData(tx_data, rx_data);
		this->fuses[i].current = uint16_t(rx_data[1]) << 4 | uint16_t(rx_data[2]) >> 4;
	}

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

/// handle timer only
SmartFuseState SmartFuse::handle_timer(void)
{
	if(watch_dog.getPassedTime() >= 40)
	{
		std::array < uint8_t, 3 > tx_data { 0, 0, 0 };
		std::array < uint8_t, 3 > rx_data { 0, 0, 0 };

		this->toggle = !this->toggle;
		tx_data[0] = READ_RAM(0x13);
		transmitReceiveData(tx_data, rx_data);
		rx_data[2] &= ~(1 << 1);
		modifyTab(tx_data, WRITE_RAM(0x13), rx_data[1], rx_data[2] |= (toggle << 1));
		transmitReceiveData(tx_data, rx_data);
		this->watch_dog.restart();

		this->state = checkGSB(rx_data[2]);
	}

	return this->state;
}

SmartFuseState SmartFuse::setFuseDutyCykle(FuseNumber fuse, uint16_t duty_cykle)
{
	uint8_t hbit = uint8_t(duty_cykle >> 8);
	uint8_t lbit = uint8_t(duty_cykle << 4) | this->toggle << 1;

	std::array < uint8_t, 3 >  tx_data { 0, 0, 0 };
	std::array < uint8_t, 3 >  rx_data { 0, 0, 0 };

	modifyTab(tx_data, WRITE_RAM(0x00 + int(fuse)), hbit, lbit);
	this->transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data[0]);
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

	this->state = checkGSB(rx_data[0]);
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

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::activeAllFuses(void)
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < 6; i++) this->fuses[i].active = true;
	modifyTab(tx_data, WRITE_RAM(0x13), 0x3f, this->toggle << 1);
	this->transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::deactivateAllFuses(void)
{
	std::array < uint8_t, 3 >  tx_data;
	std::array < uint8_t, 3 >  rx_data;

	for(int i = 0; i < 6; i++)
		this->fuses[i].active = false;
	modifyTab(tx_data, WRITE_RAM(0x13), 0x00, this->toggle << 1);
	this->transmitReceiveData(tx_data, rx_data);

//	modifyTab(tx_data, READ_RAM(0x13), 0x00, 0x00);
//	this->transmitReceiveData(tx_data, rx_data);

	this->state = checkGSB(rx_data[0]);
	return this->state;
}

SmartFuseState SmartFuse::getSmartFuseState(void)
{
	return this->state;
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
bool SmartFuseHandler<num_of_sf>::handle_all()
{
	bool result = true;

	for(auto smart_fuse : smart_fuses)
		if(smart_fuse.handle() != SmartFuseState::Ok) result = false;

	return result;
}

template <uint32_t num_of_sf>
bool SmartFuseHandler<num_of_sf>::init_all()
{
	bool result = true;

	int buff = 0;

	for(auto smart_fuse : smart_fuses)
	{
		for(int i = 0; i < buff; i++) this->smart_fuses[i].handle_timer();
		if(smart_fuse.init() != SmartFuseState::Ok) result = false;
		buff++;
	}

	this->handle_all();

	return result;
}

