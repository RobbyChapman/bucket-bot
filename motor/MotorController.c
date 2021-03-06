#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "MotorController.h"
#include <errno.h>
#include <string.h>

#define SERIAL_ERROR -9999

static const unsigned char PROTOCOL_ID = 0xAA;
static const unsigned char FORWARD = 0x05;
static const unsigned char REVERSE = 0x06;
static const unsigned char SAFE_START = 0x03;

void setMotorTermConfig(int fd);

int openMotorUart(const char *port);

int openMotorUart(const char *port) {

    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        printf("\n Error4!");
        exit(1);
    }
    return fd;
}

void setMotorTermConfig(int fd) {

    struct termios options;

    tcgetattr(fd, &options);
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tcsetattr(fd, TCSANOW, &options);
}

int initMotorController(const char *port) {

    int fileDescriptor = openMotorUart(port);
    setMotorTermConfig(fileDescriptor);

    return fileDescriptor;
}

int setExitSafeStart(int fd, unsigned char id) {

    unsigned char bytes[3] = {PROTOCOL_ID, id, SAFE_START};

    if (write(fd, bytes, sizeof(bytes)) == -1) {
        printf("\n Error !");
        return SERIAL_ERROR;
    }
    return 0;
}

int setMotorSpeedWithId(int fd, int speed, unsigned char id) {

    unsigned char bytes[5] = {PROTOCOL_ID, id, FORWARD, 0, 0};

    if (speed < 0) {
        bytes[2] = REVERSE;
        speed = -speed;
    } else {
        bytes[2] = FORWARD;
    }

    bytes[3] = (unsigned char) (speed & 0x1F);
    bytes[4] = (unsigned char) (speed >> 5 & 0x7F);

    if (write(fd, bytes, sizeof(bytes)) == -1) {
        printf("Error on write() %s \n", strerror(errno));
        return SERIAL_ERROR;
    }
    return 0;
}