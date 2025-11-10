#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "uart_ioctl.h"

#define UART_DEVICE "/dev/evlsi_uart_device"

#define BAUDRATE_9600 9600
#define BAUDRATE_115200 115200

int main()
{
    int fd;
    int status;
    int ret;
    int32_t value=12334;

    // Open the UART device
    fd = open(UART_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open UART device");
        return EXIT_FAILURE;
    }

    ret = ioctl(fd, WRITE_REGISTERS, (int *)&value);
    if (ret >= 0) {
        printf("WRITE_REGISTERS IOCTL call successful\n");
    } else {
        perror("Failed to write registers via IOCTL");
        goto error;
    }
    ret = ioctl(fd, READ_REGISTERS, (int *)&value);
    if (ret >= 0) {
        printf("READ_REGISTERS IOCTL call successful : %d\n", value);
    } else {
        perror("Failed to read registers via IOCTL");
        goto error;
    }

    // Close the UART device
    close(fd);
    return EXIT_SUCCESS;
error:
    close(fd);
    return EXIT_FAILURE;
}
