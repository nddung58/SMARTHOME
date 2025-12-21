#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdint.h>
#include "hal/gpio_types.h"

void output_io_init(gpio_num_t gpio_num);
void output_io_set_level(gpio_num_t gpio_num, uint32_t level);
void output_io_toggle(gpio_num_t gpio_num);

uint8_t output_io_getstatus(gpio_num_t);

#endif // OUTPUT_H