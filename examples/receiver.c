#include "../receiver/XbeeReceiver.h"

int main(int argc, char *argv[]) {

    char *t = "/dev/ttyTHS1";
    init(t, 115200);

    return 0;
}