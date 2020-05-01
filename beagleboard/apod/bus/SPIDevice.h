/*
 * SPIDevice.h  Created on: 22 May 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring BeagleBone" 
 * See: www.exploringbeaglebone.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European 
 * Union Public License (the "EUPL") version 1.1 as published by the 
 * European Union. Any use of this Software, other than as authorized 
 * under this License is strictly prohibited (to the extent such use 
 * is covered by a right of the copyright holder of this Software).
 * 
 * This Software is provided under the License on an "AS IS" basis and 
 * without warranties of any kind concerning the Software, including 
 * without limitation merchantability, fitness for a particular purpose, 
 * absence of defects or errors, accuracy, and non-infringement of 
 * intellectual property rights other than copyright. This disclaimer 
 * of warranty is an essential part of the License and a condition for 
 * the grant of any rights to this Software.
 * 
 * For more details, see http://www.derekmolloy.ie/
 */

#ifndef SPIDEVICE_H_
#define SPIDEVICE_H_
#include<string>
#include<stdint.h>
#include <linux/spi/spidev.h>

#define SPI_PATH "/dev/spidev"

/**
 * @class SPIDevice
 * @brief Generic SPI Device class that can be used to connect to any type of SPI device and read or write to its registers
 */
class SPIDevice {
public:
	/// The SPI Mode
	enum SPIMODE{
		MODE0 = 0,   //!< Low at idle, capture on rising clock edge
		MODE1 = 1,   //!< Low at idle, capture on falling clock edge
		MODE2 = 2,   //!< High at idle, capture on falling clock edge
		MODE3 = 3    //!< High at idle, capture on rising clock edge
	};

private:
	std::string filename;

public:
	SPIDevice(unsigned int bus, unsigned int device);
  ~SPIDevice();
	int open();
	void debugDump();
	int write(unsigned char value);
	int write(unsigned char value[], int length);
	int setSpeed(uint32_t speed);
	int setLSBFirst(uint8_t lsbFirst);
	int setMode(SPIDevice::SPIMODE mode);
	int setBitsPerWord(uint8_t bits);
  int transfer(const uint8_t send[], uint8_t receive[], int len);
	void close();

private:
  unsigned int bus;
  unsigned int device;
  int file;
	SPIMODE mode;
	uint8_t bits;
	uint8_t lsbFirst;
	uint32_t speed;
	uint16_t delay;
  struct spi_ioc_transfer	ioc_transfer;
  uint8_t buf_transfer[0x100];
};

#endif /* SPIDEVICE_H_ */
