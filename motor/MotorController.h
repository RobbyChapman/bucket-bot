/**
 * Created by Robby Chapman on 06/27/16.
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <termios.h>

static const unsigned char LEFT_MOTOR = 0x0A;
static const unsigned char RIGHT_MOTOR = 0x0C;

int initMotorController(const char *port);
int setExitSafeStart(int fd, unsigned char id);
int setMotorSpeedWithId(int fd, int speed, unsigned char id);

#endif