#include "../receiver/XbeeReceiver.h"
#include <stdio.h>

void rxHandler(struct N64_DTO controller);

int main(int argc, char *argv[]) {

    char *t = "/dev/ttyTHS1";
    initWithRxHandler(t, 115200, rxHandler);

    return 0;
}

void rxHandler(struct N64_DTO controller) {

    printf("\n\n Y Joystick:: %i \n", (signed char) controller.y);
    printf("X Joystick:: %i \n\n", (signed char) controller.x);
}