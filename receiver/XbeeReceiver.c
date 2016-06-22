#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include "N64_DTO.h"

char *DEFAULT_PORT = "/dev/ttyTHS1";
int DEFAULT_BAUD = B115200;

int fileDescriptor = -1;

struct N64_DTO queryController(int fd);
void init(char *port, int baud);
void setTermConfig(int fd, int baud);
int openUART(char *port);

void init(char *port, int baud) {

    fileDescriptor = openUART(DEFAULT_PORT);
    setTermConfig(fileDescriptor, DEFAULT_BAUD);

    while (true) {
        queryController(fileDescriptor);
       // printf("Y Joystick:: %i \n", (signed char) controller.y);
       // printf("X Joystick:: %i \n", (signed char) controller.x);
    }
}

struct N64_DTO queryController(int fd) {

    struct N64_DTO controller;
    int byteCount = read(fd, (char *) &controller, sizeof(struct N64_DTO));

    if (byteCount == -1) {
        printf("We have an error!");
    } else {
        printf("%i btyes recieved...\n", byteCount);
    }
    return controller;
}

void setTermConfig(int fd, int baud) {

    struct termios toptions;

    tcgetattr(fd, &toptions);
    cfsetspeed(&toptions, baud);

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
}

int openUART(char *port) {

    return open(port, O_RDWR | O_NOCTTY);
}

int main(int argc, char *argv[]) {

    char *t = "/dev/ttyTHS1";
    init(t, B115200);

    return 0;
}
