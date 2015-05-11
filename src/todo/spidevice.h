#ifndef SPIDEVICE_H
#define SPIDEVICE_H

#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <cstdint>
#include <cstdlib>

class SPIDevice
{
public:
	SPIDevice();

private:
	const char *device_ = "/dev/spidev0.0";
	int fd_;
	uint8_t mode_;
	uint8_t bits_;
	uint32_t speed_;
	uint16_t delay_;
};

// FIXME this should not be a global!!!
extern SPIDevice atmega;

#endif // SPIDEVICE_H
