#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define SERIAL_ERROR -9999

int smcExitSafeStart(int fd, int id) {

    unsigned char command[3];
    command[0] = 0xAA;
    command[1] = id;
    command[2] = 0x03;

    if (write(fd, command, sizeof(command)) == -1) {
        perror("error writing");
        return SERIAL_ERROR;
    }
    return 0;
}

int smcSetTargetSpeed(int fd, int speed, int id) {

    unsigned char command[5];
    printf("Is is %i \n", id);
    command[0] = 0xAA;
    command[1] = id;
    command[2] = 0x05;
    command[3] = speed & 0x1F;
    command[4] = speed >> 5 & 0x7F;

    if (write(fd, command, sizeof(command)) == -1) {
        perror("error writing");
        return SERIAL_ERROR;
    }
    return 0;
}

int main() {

    const char *device = "/dev/ttyTHS0";
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror(device);
        return 1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tcsetattr(fd, TCSANOW, &options);

    sleep(1);
    smcExitSafeStart(fd, 12);
    smcExitSafeStart(fd, 10);

    sleep(1);

    smcSetTargetSpeed(fd, 800, 12);
    smcSetTargetSpeed(fd, 800, 10);

    sleep(2);

    smcSetTargetSpeed(fd, 0, 12);
    smcSetTargetSpeed(fd, 0, 10);
    sleep(2);

    close(fd);

    return 0;
}