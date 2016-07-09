#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "MotorController.h"

#define SERIAL_ERROR -9999

static const unsigned char PROTOCOL_ID = 0xAA;
static const unsigned char FORWARD = 0x05;
static const unsigned char REVERSE = 0x06;
static const unsigned char SAFE_START = 0x03;

void setTermConfig(int fd);
int openUART(const char *port);
int writeBytes(int fd, unsigned char *command);

int openUART(const char *port) {

    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        exit(1);
    }
    return fd;
}

void setTermConfig(int fd) {

    struct termios options;

    tcgetattr(fd, &options);
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tcsetattr(fd, TCSANOW, &options);
}

int writeBytes(int fd, unsigned char *command) {

    if (write(fd, command, sizeof(command)) == -1) {
        perror("error writing");
        return SERIAL_ERROR;
    }
    return 0;
}

int initMotorController(const char *port) {

    int fileDescriptor = openUART(port);
    setTermConfig(fileDescriptor);

    return fileDescriptor;
}

int setExitSafeStart(int fd, unsigned char id) {

    unsigned char bytes[3] = {PROTOCOL_ID, id, SAFE_START};
    return writeBytes(fd, bytes);
}

int setMotorSpeedWithId(int fd, int speed, unsigned char id) {

    unsigned char bytes[5] = {PROTOCOL_ID, id, FORWARD, (unsigned char) (speed & 0x1F), (unsigned char) (speed >> 5 & 0x7F)};

    if (write(fd, bytes, sizeof(bytes)) == -1) {
        perror("error writing");
        return SERIAL_ERROR;
    }
    return 0;
}


