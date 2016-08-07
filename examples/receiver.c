/**
 * Created by Robby Chapman on 06/22/16.
 *
 * gcc receiver.c ../receiver/XbeeReceiver.c -pthread -o receiver
 *
 */

#include "../receiver/XbeeReceiver.h"
#include <stdio.h>

void rxHandler(struct N64_DTO controller);

int main(int argc, char *argv[]) {

    char *port = "/dev/ttyTHS1";
    int fileDescriptor = initWithRxHandler(port, 115200, rxHandler);

    while (true) {
        char input = (char) getchar();
        if (input == 'q') {
            printf("\n Quiting application \n\n");
            close(fileDescriptor);
            break;
        }
    }
    return 0;
}

void rxHandler(struct N64_DTO controller) {

    printf("\n\n Y Joystick:: %i \n", (signed char) controller.y);
    printf("X Joystick:: %i \n\n", (signed char) controller.x);
}
