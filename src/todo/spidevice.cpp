#include "spidevice.h"
#include <cstdio>

SPIDevice::SPIDevice() : bits_(8), speed_(500000)
{
    fd_ = open(device_, O_RDWR);
    if (fd_ == -1)
    {
        perror("SPIDevice open()");
        // TODO DONT DIE, throw new exception
        exit(EXIT_FAILURE);
    }
    
    int ret = ioctl(fd_, SPI_IOC_WR_MODE, &mode_);
    if (ret == -1)
    {
        perror("ioctl 1 failed");
        exit(EXIT_FAILURE);
    }
    
    ret = ioctl(fd_, SPI_IOC_RD_MODE, &mode_);
    if (ret == -1)
    {
        perror("ioctl 2 failed");
        exit(EXIT_FAILURE);
    }
    
    ret = ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_);
    if (ret == -1)
    {
        perror("ioctl 3 failed");
        exit(EXIT_FAILURE);
    }
    
    ret = ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_);
    if (ret == -1)
    {
        perror("ioctl 4 failed");
        exit(EXIT_FAILURE);
    }
    
    ret = ioctl(fd_, SPI_IOC_RD_MAX_SPEED_HZ, &speed_);
    if (ret == -1)
    {
        perror("ioctl 5 failed");
        exit(EXIT_FAILURE);
    }
}

SPIDevice atmega; // global SPIDevice for ATMega communications
