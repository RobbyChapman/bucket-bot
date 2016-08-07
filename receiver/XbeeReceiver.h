/**
 * Created by Robby Chapman on 06/20/16.
 */

#include <termios.h>
#include "N64_DTO.h"

#ifndef XBEE_RECEIVER_H
#define XBEE_RECEIVER_H

struct N64_DTO queryController(void *x_void_ptr);
int initWithRxHandler(const char *port, const speed_t baud, void (*rxCallback)(struct N64_DTO));

#endif

