#ifndef BSW_DOOR_H_
#define BSW_DOOR_H_

#include "stdint.h"
#include "stdbool.h"

void door_init(void);

bool door_open(void);
bool door_close(void);

uint8_t door_getstate(void);

#endif // BSW_DOOR_H_