#include "BotUtils.h"
#include <stdio.h>
#include <math.h>

int normalizeMotorSpeed(int speed, NormalizedMotorConfig config) {

    return ceil(config.minMotorCtrlInput +
                     (speed - config.minCtrlOutput) * (config.maxMotorCtrlInput - config.minMotorCtrlInput) /
                     (config.maxCtrlOutput - config.minCtrlOutput));
}

BiMotorPositions transformedMotorPositions(Joystick joystick) {

    printf("Hello world 1? \n");
}