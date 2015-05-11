#ifndef SPIDEVICE_H
#define SPIDEVICE_H

#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <cstdint>

class SPIDevice
{
public:

private:
	int fd;
};

#endif // SPIDEVICE_H
