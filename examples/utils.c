/**
 * Created by Robby Chapman on 07/9/16.
 *
 * gcc ../utils/BotUtils.c utils.c -lm -o utils
 *
 */

#include "../utils/BotUtils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    int normalizeMotorSpeed(int speed, NormalizedMotorConfig config);

    NormalizedMotorConfig config;

    config.minCtrlOutput = -81;
    config.maxCtrlOutput = 81;
    config.minMotorCtrlInput = -3200;
    config.maxMotorCtrlInput = 3200;

    int i = config.minCtrlOutput;

    for (; i <= config.maxCtrlOutput; i++) {
        printf("Normalized speed is: %i \n", normalizeMotorSpeed(i, config));
    }

    return 0;
}