#include "../receiver/XbeeReceiver.h"
#include "../motor/MotorController.h"
#include "../utils/BotUtils.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void rxHandler(struct N64_DTO controller);
static NormalizedMotorConfig config;
static int fileDescriptor = -1;

int main(int argc, char *argv[]) {

    char *port = "/dev/ttyTHS1";
    fileDescriptor = initWithRxHandler(port, 115200, rxHandler);

    if (fileDescriptor > 0) {
        memset(&config, 0, sizeof(config));
        config.minCtrlOutput = -91;
        config.maxCtrlOutput = 91;
        config.minMotorCtrlInput = -3200;
        config.maxMotorCtrlInput = 3200;

        port = "/dev/ttyTHS0";
        fileDescriptor = initMotorController(port);
        setExitSafeStart(fileDescriptor, RIGHT_MOTOR);
        setExitSafeStart(fileDescriptor, LEFT_MOTOR);
    }

    while (true) {
        char input = (char) getchar();
        if (input == 'q') {
            printf("\n Quiting application \n\n");
            setMotorSpeedWithId(fileDescriptor, 0, RIGHT_MOTOR);
            setMotorSpeedWithId(fileDescriptor, 0, LEFT_MOTOR);
            setExitSafeStart(fileDescriptor, RIGHT_MOTOR);
            setExitSafeStart(fileDescriptor, LEFT_MOTOR);
            close(fileDescriptor);
            break;
        }
    }

    return 0;
}

void rxHandler(struct N64_DTO controller) {

    Joystick joystick;
    memset(&joystick, 0, sizeof(joystick));

    joystick.x = (int) normalizeMotorSpeed(controller.x, config);
    joystick.y = (int) normalizeMotorSpeed(controller.y, config);

    printf("\n Original motor speeds are left: %i right: %i", joystick.x, joystick.y);
    adjustVariance(&joystick);
    BiMotorPositions positions = transformedMotorPositionsV2(joystick);
    printf("\n Motor positions are left: %i right: %i \n", positions.left, positions.right);

    setMotorSpeedWithId(fileDescriptor, positions.left, LEFT_MOTOR);
    setMotorSpeedWithId(fileDescriptor, positions.right, RIGHT_MOTOR);
}
