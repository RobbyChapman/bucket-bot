#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include "N64_DTO.h"

const char *UART_PORT = "/dev/ttyTHS1";
const int BAUD = B115200;

int main(int argc, char *argv[]) {

    int fd = open(UART_PORT, O_RDWR | O_NOCTTY);
    struct termios toptions;
    struct N64_DTO controller;

    tcgetattr(fd, &toptions);
    cfsetspeed(&toptions, BAUD);

    /* man termios for individual definitions */
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    toptions.c_cflag &= ~CRTSCTS;
    toptions.c_cflag |= CREAD | CLOCAL;
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    toptions.c_oflag &= ~OPOST;
    toptions.c_cc[VMIN] = sizeof(struct N64_DTO);
    toptions.c_cc[VTIME] = 0;

    tcsetattr(fd, TCSANOW, &toptions);
    tcflush(fd, TCIFLUSH);

    while (true) {
        int n = read(fd, (char *) &controller, sizeof(struct N64_DTO));

        printf("%i btyes recieved...\n", n);
        printf("Y Joystick:: %i \n", (signed char) controller.y);
        printf("X Joystick:: %i \n", (signed char) controller.x);
    }
    return 0;
}
