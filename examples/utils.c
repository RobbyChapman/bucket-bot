/**
 * Created by Robby Chapman on 07/9/16.
 *
 * gcc ../utils/BotUtils.c utils.c -lm -o utils
 *
 */

#include "../utils/BotUtils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    double normalizeMotorSpeed(int speed, NormalizedMotorConfig config);

    NormalizedMotorConfig config;

    config.minCtrlOutput = -81;
    config.maxCtrlOutput = 81;
    config.minMotorCtrlInput = -3200;
    config.maxMotorCtrlInput = 3200;

    int i = config.minCtrlOutput;

    for (; i <= config.maxCtrlOutput; i++) {
        printf("Normalized speed is: %d \n", (int) normalizeMotorSpeed(i, config));
    }

    Joystick joystick;
    joystick.x = (int) normalizeMotorSpeed(67, config);
    joystick.y = (int) normalizeMotorSpeed(-72, config);

    printf("Original motor speeds are left: %d right: %i \n", joystick.x, joystick.y);
    BiMotorPositions positions = transformedMotorPositions(joystick);
    printf("Motor positions are left: %d right: %i \n", positions.left, positions.right);

    return 0;
}