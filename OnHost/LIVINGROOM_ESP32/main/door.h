#ifndef MAIN_DOOR_H__
#define MAIN_DOOR_H__

#include <stdint.h>
#include "esp_err.h"
#include "stdbool.h"

void door_init(void);

bool door_open(void);
bool door_close(void);

#endif // MAIN_DOOR_H_
