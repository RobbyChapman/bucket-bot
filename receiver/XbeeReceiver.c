#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "XbeeReceiver.h"

#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

void signal_handler_IO(int status);

int wait_flag = TRUE;
int fileDescriptor = -1;

typedef void (*RxHandlerFn)(struct N64_DTO controller);

typedef struct RXConfig {
    int fileDescriptor;
    RxHandlerFn rxHandlerFn;
} RXConfig;

/* Constants */
const char *DEFAULT_PORT = "/dev/ttyTHS1";
const int DEFAULT_BAUD = B115200;

/* Prototypes */

struct N64_DTO queryController(void *x_void_ptr);

void initWithRxHandler(const char *port, const int baud, void (*rxCallback)(struct N64_DTO));

void setTermConfig(int fd, const int baud);

void signal_handler_IO(int status);

int openUART(const char *port);

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
        printf("I'll handle this error later on");
        exit(1);
    }

    x_ptr->rxHandlerFn(controller);

    return controller;
}

void initWithRxHandler(const char *port, const int baud, void (*rxCallback)(struct N64_DTO)) {

    fileDescriptor = openUART(port ? port : DEFAULT_PORT);
    setTermConfig(fileDescriptor, baud ? baud : DEFAULT_BAUD);

    pthread_t readXbeeThread;

    RXConfig temp;
    temp.rxHandlerFn = rxCallback;
    temp.fileDescriptor = fileDescriptor;

    if (pthread_create(&readXbeeThread, NULL, pollRxThreadFn, &temp)) {
        fprintf(stderr, "Error creating thread\n");
        exit(1);
    }

    while (STOP == FALSE) {

        usleep(2000);
        printf("hello \n");
        if (wait_flag == FALSE) {
            //struct N64_DTO controller = queryController(fileDescriptor);
            //printf("\n\n\n Y Joystick:: %i \n", (signed char) controller.y);
            //printf("X Joystick:: %i \n", (signed char) controller.x);
            /* I am supposed to set STOP to TRUE for this loop to stop, however I always want to read from the controller,
             which is why this is now blocking */
            //    wait_flag = TRUE;
        }
    }
}

void signal_handler_IO(int status) {
    printf("received SIGIO signal.\n");
    wait_flag = FALSE;
}

void setTermConfig(int fd, const int baud) {

    struct sigaction saio;
    saio.sa_handler = signal_handler_IO;
    sigemptyset(&saio.sa_mask);
    sigaddset(&saio.sa_mask, SIGINT);
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO, &saio, NULL);
    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETFL, FASYNC);

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

int openUART(const char *port) {

    return open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
}