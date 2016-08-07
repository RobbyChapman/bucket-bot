/**
 * Created by Robby Chapman on 07/09/16.
 */

#ifndef BOT_UTILS_H
#define BOT_UTILS_H

typedef struct {
    int left;
    int right;
} BiMotorPositions;

typedef struct {
    int x;
    int y;
} Joystick;

typedef struct {
    int minCtrlOutput;
    int maxCtrlOutput;
    int minMotorCtrlInput;
    int maxMotorCtrlInput;
} NormalizedMotorConfig;

double normalizeMotorSpeed(int speed, NormalizedMotorConfig config);
BiMotorPositions transformedMotorPositions(Joystick joystick);
void adjustVariance(Joystick *joystick);
BiMotorPositions transformedMotorPositionsV2(Joystick joystick);

#endif