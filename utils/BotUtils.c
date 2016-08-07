/**
 * Created by Robby Chapman on 07=/09/16.
 */
#include "BotUtils.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_SPEED 3200

static void setStickDelta(int *stick);

double normalizeMotorSpeed(int speed, NormalizedMotorConfig config) {

    return ceil(config.minMotorCtrlInput +
                (speed - config.minCtrlOutput) * (config.maxMotorCtrlInput - config.minMotorCtrlInput) /
                (config.maxCtrlOutput - config.minCtrlOutput));
}

BiMotorPositions transformedMotorPositions(Joystick joystick) {

    BiMotorPositions positions;

    int nJoyY = joystick.y;
    int nJoyX = joystick.x;

    int nMotMixL;
    int nMotMixR;

    float fPivYLimit = 1.0;
    float ABS_SPEED = 3200.0;

    int nMotPremixL, nMotPremixR, nPivSpeed, fPivScale;

    if (nJoyY >= 0) {
        nMotPremixL = (int) ((nJoyX >= 0) ? ABS_SPEED : (ABS_SPEED + nJoyX));
        nMotPremixR = (int) ((nJoyX >= 0) ? (ABS_SPEED - nJoyX) : ABS_SPEED);
    } else {
        nMotPremixL = (int) ((nJoyX >= 0) ? (ABS_SPEED - nJoyX) : ABS_SPEED);
        nMotPremixR = (int) ((nJoyX >= 0) ? ABS_SPEED : (ABS_SPEED + nJoyX));
    }

    nMotPremixL = (int) (nMotPremixL * nJoyY / ABS_SPEED);
    nMotPremixR = (int) (nMotPremixR * nJoyY / ABS_SPEED);

    nPivSpeed = nJoyX;
    fPivScale = (int) ((fabs(nJoyY) > fPivYLimit) ? 0.0 : (1.0 - fabs(nJoyY) / fPivYLimit));

    nMotMixL = (int) ((1.0 - fPivScale) * nMotPremixL + fPivScale * (nPivSpeed));
    nMotMixR = (int) ((1.0 - fPivScale) * nMotPremixR + fPivScale * (-nPivSpeed));

    positions.left = nMotMixL;
    positions.right = nMotMixR;

    return positions;
}

BiMotorPositions transformedMotorPositionsV2(Joystick joystick) {

    BiMotorPositions positions;
    memset(&positions, 0, sizeof(positions));
    
    double Y = joystick.y;
    double X = -joystick.x;
    double V = (MAX_SPEED-(fabs(X))) * (Y/MAX_SPEED) + Y;
    double W = (MAX_SPEED-(fabs(Y))) * (X/MAX_SPEED) + X;
    double R = (V+W) /2;
    double L = (V-W)/2;

    positions.right = (int) R;
    positions.left = (int) L;

    return positions;
}

static void setStickDelta(int *stick) {

    double stickVariance = -100;
    if ((*stick <= 0 && *stick >= stickVariance) || (*stick >= 0 && *stick <= fabs(stickVariance))) {
        *stick = 0;
    }
}

void adjustVariance(Joystick *joystick) {

    setStickDelta(&joystick->x);
    setStickDelta(&joystick->y);
}