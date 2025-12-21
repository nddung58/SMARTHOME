#include <stdio.h>
#include <driver/gpio.h>
#include "esp_attr.h"
#include "input.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

input_callback_t input_callback = NULL;
static uint64_t _start, _stop, _pressTick;

static void IRAM_ATTR gpio_input_handler(void *arg)
{
    int gpio_num = (uint32_t)arg;

    uint64_t rtc = xTaskGetTickCountFromISR();

    if (gpio_get_level(gpio_num) == 0)
    {
        // Button pressed
        _start = rtc;
    }
    else
    {
        // Button released
        _stop = rtc;
        _pressTick = _stop - _start;
        if (input_callback != NULL)
        {
            input_callback(gpio_num, _pressTick);
        }
    }
}

// static void IRAM_ATTR gpio_input_handler(void *arg)
// {
//     int gpio_num = (uint32_t)arg;
//     input_callback(gpio_num);
// }

void input_io_create(gpio_num_t gpio_num, interrupt_type_edge_t type)
{
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio_num, type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_input_handler, (void *)gpio_num);
}

uint8_t input_io_get_level(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num);
}

void input_set_callback(void *cb)
{
    input_callback = cb;
}