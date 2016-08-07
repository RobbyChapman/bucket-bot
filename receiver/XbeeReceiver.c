/**
 * Created by Robby Chapman on 06/20/16.
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "XbeeReceiver.h"
#include <errno.h>
#include <string.h>

struct termios originalConfig;
int fileDescriptor = -1;

/* Types */
typedef void (*RxHandlerFn)(struct N64_DTO controller);
typedef struct RXConfig {
    int fileDescriptor;
    RxHandlerFn handlerFn;
} RXConfig;

/* Constants */
const char *DEFAULT_PORT = "/dev/ttyTHS1";
const speed_t DEFAULT_BAUD = B115200;

/* Prototypes */
struct N64_DTO queryController(void *x_void_ptr);
int initWithRxHandler(const char *port, const speed_t baud, void (*rxCallback)(struct N64_DTO));
void setTermConfig(int fd, const speed_t baud);
int openUART(const char *port);
void setOriginalTermConfig(int fd, struct termios config);
void setFileDescriptor(const char *port);
void *pollRxThreadFn(void *x_void_ptr);

int initWithRxHandler(const char *port, const speed_t baud, void (*rxCallback)(struct N64_DTO)) {

    setFileDescriptor(port);
    setOriginalTermConfig(fileDescriptor, originalConfig);
    setTermConfig(fileDescriptor, baud ? baud : DEFAULT_BAUD);

    static RXConfig rxConfig;
    memset(&rxConfig, 0, sizeof(rxConfig));

    rxConfig.handlerFn = rxCallback;
    rxConfig.fileDescriptor = fileDescriptor;

    pthread_t readXbeeThread;
    if (pthread_create(&readXbeeThread, NULL, pollRxThreadFn, &rxConfig)) {
        fprintf(stderr, "Error creating thread\n");
        exit(1);
    }

    return fileDescriptor;
}

int openUART(const char *port) {

    return open(port, O_RDWR | O_NOCTTY);
}

void *pollRxThreadFn(void *x_void_ptr) {

    while (true) {
        queryController(x_void_ptr);
    }
    return NULL;
}

struct N64_DTO queryController(void *x_void_ptr) {

    struct RXConfig *x_ptr = (struct RXConfig *) x_void_ptr;
    struct N64_DTO controller;

    ssize_t byteCount = read(x_ptr->fileDescriptor, (char *) &controller, sizeof(struct N64_DTO));

    if (byteCount == -1) {
        printf("Error on read() %s \n", strerror(errno));
        exit(1);
    }

    x_ptr->handlerFn(controller);

    return controller;
}

void setOriginalTermConfig(int fd, struct termios config) {

    tcgetattr(fd, &config);
}

void setFileDescriptor(const char *port) {

    fileDescriptor = openUART(port ? port : DEFAULT_PORT);
}

void setTermConfig(int fd, const speed_t baud) {

    struct termios toptions, oldOptions;

    tcgetattr(fd, &oldOptions);
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