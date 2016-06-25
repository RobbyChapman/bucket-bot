/**
 * Created by Robby Chapman on 06/20/16.
 */

#include "N64_DTO.h"

#ifndef XBEE_RECEIVER_H
#define XBEE_RECEIVER_H

struct N64_DTO queryController(void *x_void_ptr);
void initWithRxHandler(const char *port, const int baud, void (*rxCallback)(struct N64_DTO));

#endif

