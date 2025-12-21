#ifndef MAIN_SG90_H_
#define MAIN_SG90_H_

#include <stdint.h>
#include <stdbool.h>

void sg90_init(void);
bool sg90_set_angle(uint8_t angle);
#endif // MAIN_SG90_H_