/**
* Created by robert.chapman on 4/21/16.
*/

#ifndef N64_DTO_H
#define N64_DTO_H

#include <stdbool.h>

struct N64_DTO {
    bool a;
    bool b;
    char x;
    char y;
    bool z;
    bool start;
    bool cUp;
    bool cDown;
    bool cLeft;
    bool cRight;
    bool dUp;
    bool dDown;
    bool dLeft;
    bool dRight;
    bool leftBumper;
    bool rightBumper;
};

#endif
