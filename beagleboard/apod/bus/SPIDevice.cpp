/*
 * SPIDevice.cpp  Created on: 22 May 2014
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

#include "SPIDevice.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
using namespace std;

#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)

#define REVERSE_BIT(x) ((((x) & 0x80) >> 7) | (((x) & 0x40) >> 5) | (((x) & 0x20) >> 3) | (((x) & 0x10) >> 1) | (((x) & 0x08) << 1) | (((x) & 0x04) << 3) | (((x) & 0x02) << 5) | (((x) & 0x01) << 7))

/**
 * The constructor for the SPIDevice that sets up and opens the SPI connection.
 * The destructor will close the SPI file connection.
 * @param bus The SPI bus number X (first digit after spidevX.Y)
 * @param device The device on the bus Y (second digit after spidevX.Y)
 */
SPIDevice::SPIDevice(unsigned int bus, unsigned int device) {
  this->bus = bus;
  this->device = device;
	stringstream s;
	s << SPI_PATH << bus << "." << device;
	this->filename = string(s.str());
	this->mode = SPIDevice::MODE3;
	this->bits = 8;
	this->speed = 500000;
	this->delay = 100;
	this->lsbFirst = 0;
	this->open();
}

/**
 * This method opens the file connection to the SPI device.
 * @return 0 on a successful open of the file
 */
int SPIDevice::open(){
	cout << "Opening the file: " << filename.c_str() << endl;
	if ((this->file = ::open(filename.c_str(), O_RDWR))<0){
		perror("SPI: Can't open device.");
		return -1;
	}
	if (this->setMode(this->mode)==-1) return -1;
	if (this->setSpeed(this->speed)==-1) return -1;
	if (this->setBitsPerWord(this->bits)==-1) return -1;
	if (this->setLSBFirst(this->lsbFirst)==-1) return -1;
	return 0;
}

/**
 * Generic method to transfer data to and from the SPI device. It is used by the
 * following methods to read and write registers.
 * @param send The array of data to send to the SPI device
 * @param receive The array of data to receive from the SPI device
 * @param len The len of the array to send
 * @return -1 on failure
 */
int SPIDevice::transfer(const uint8_t send[], uint8_t receive[], int len) {
	ioc_transfer.tx_buf = (unsigned long) send;
	ioc_transfer.rx_buf = (unsigned long) receive;
	ioc_transfer.len = len;
	ioc_transfer.speed_hz = this->speed;
	ioc_transfer.bits_per_word = this->bits;
	ioc_transfer.delay_usecs = this->delay;
	ioc_transfer.cs_change = 0;
	if (this->lsbFirst) {
	  for (int i = 0; i < len; i++) {
      buf_transfer[i] = REVERSE_BIT(send[i]);
	  }
    ioc_transfer.tx_buf = (unsigned long) buf_transfer;
	}
	int status = ioctl(this->file, SPI_IOC_MESSAGE(1), &ioc_transfer);
	if (status < 0) {
		perror("SPI: SPI_IOC_MESSAGE Failed");
		return -1;
	}
	if (this->lsbFirst && receive) {
    for (int i = 0; i < len; i++) {
      receive[i] = REVERSE_BIT(receive[i]);
    }
	}
	return status;
}

int SPIDevice::write(unsigned char value){
	unsigned char null_return = 0x00;
	//printf("[%02x]", value);
	this->transfer(&value, &null_return, 1);
	return 0;
}

int SPIDevice::write(unsigned char value[], int length){
	unsigned char null_return = 0x00;
	this->transfer(value, &null_return, length);
	return 0;
}

void SPIDevice::debugDump(){
	cout << "SPI Mode: " << this->mode << endl;
	cout << "Bits per word: " << (int)this->bits << endl;
	cout << "Max speed: " << this->speed << endl;
	cout << "LSB First: " << (this->lsbFirst ? "true" : "false") << endl;
}

int SPIDevice::setSpeed(uint32_t speed){
	this->speed = speed;
	if (ioctl(this->file, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed)==-1){
		perror("SPI: Can't set max speed HZ");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed)==-1){
		perror("SPI: Can't get max speed HZ.");
		return -1;
	}
	return 0;
}

int SPIDevice::setLSBFirst(uint8_t lsbFirst) {
	this->lsbFirst = lsbFirst;
	// spidev doesn't seem to support LSB first
	return 0;
}

int SPIDevice::setMode(SPIDevice::SPIMODE mode){
	this->mode = mode;
	if (ioctl(this->file, SPI_IOC_WR_MODE, &this->mode)==-1){
		perror("SPI: Can't set SPI mode.");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_MODE, &this->mode)==-1){
		perror("SPI: Can't get SPI mode.");
		return -1;
	}
	return 0;
}

int SPIDevice::setBitsPerWord(uint8_t bits){
	this->bits = bits;
	if (ioctl(this->file, SPI_IOC_WR_BITS_PER_WORD, &this->bits)==-1){
		perror("SPI: Can't set bits per word.");
		return -1;
	}
	if (ioctl(this->file, SPI_IOC_RD_BITS_PER_WORD, &this->bits)==-1){
		perror("SPI: Can't get bits per word.");
		return -1;
	}
	return 0;
}

void SPIDevice::close(){
	::close(this->file);
	this->file = -1;
}

SPIDevice::~SPIDevice() {
	this->close();
}
