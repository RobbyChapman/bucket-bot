#include "N64_DTO.h"

#ifndef XBEE_RECEIVER_H
#define XBEE_RECEIVER_H

struct N64_DTO queryController(int fd);
void init(const char *port, const int baud);

#endif

