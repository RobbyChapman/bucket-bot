/**
 * Created by Robby Chapman on 07/09/16.
 *
 * gcc ../motor/MotorController.c motor.c -o motor
 *
 */

#include "../motor/MotorController.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    char *port = "/dev/ttyTHS0";
    int fd = initMotorController(port);

    setExitSafeStart(fd, LEFT_MOTOR);
    setExitSafeStart(fd, RIGHT_MOTOR);
    sleep(2);
    setMotorSpeedWithId(fd, 500, LEFT_MOTOR);
    setMotorSpeedWithId(fd, 500, RIGHT_MOTOR);
    sleep(5);
    setMotorSpeedWithId(fd, 0, LEFT_MOTOR);
    setMotorSpeedWithId(fd, 0, RIGHT_MOTOR);

    return 0;
}