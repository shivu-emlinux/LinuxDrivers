#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

static  struct spi_ioc_transfer txbuffer;

int main()
{
    int fd, ret;
    int desiredFreequency=2000000;
    int SpiMode = SPI_MODE_0;
    long unsigned defaultFreq;

    fd = open("/dev/evlsi_spi_device", O_RDWR);
    printf("fd: %d\n", fd);
    ret = ioctl(fd, SPI_IOC_WR_MODE, &SpiMode);

    //ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &defaultFreq);
    //printf("current Frequency:%d %lu\n", ret, defaultFreq);
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &desiredFreequency);

    printf("after modification Frequency: %lu\n", defaultFreq);

    memset(&txbuffer, 0, sizeof(txbuffer));
    txbuffer.speed_hz = desiredFreequency;
    txbuffer.cs_change = 0;
    txbuffer.delay_usecs = 0;
    txbuffer.bits_per_word = 8;

    printf("Setting the data\n");


}