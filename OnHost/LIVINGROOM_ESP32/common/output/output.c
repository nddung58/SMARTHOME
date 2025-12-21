#include <stdio.h>
#include <driver/gpio.h>
#include "output.h"

void output_io_init(gpio_num_t gpio_num)
{
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
}

void output_io_set_level(gpio_num_t gpio_num, uint32_t level)
{
    gpio_set_level(gpio_num, level);
}

void output_io_toggle(gpio_num_t gpio_num)
{
    uint32_t old_level = gpio_get_level(gpio_num);
    gpio_set_level(gpio_num, 1U - old_level);
}

uint8_t output_io_getstatus(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num);
}