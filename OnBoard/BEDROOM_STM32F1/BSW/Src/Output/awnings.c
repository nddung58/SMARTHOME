#include "awnings.h"
#include "stm32_gpio.h"
#include "timer_base.h"

#define AWNINGS_GPIO_PORT GPIOA
#define AWNINGS_GPIO_PIN (1U << 4) // PA4

void awnings_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = AWNINGS_GPIO_PIN,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_HIGH};
    GPIO_Init(AWNINGS_GPIO_PORT, &gpio);
    GPIO_WritePin(AWNINGS_GPIO_PORT, AWNINGS_GPIO_PIN, GPIO_PIN_SET);
}

void awnings_open(void)
{
    GPIO_WritePin(AWNINGS_GPIO_PORT, AWNINGS_GPIO_PIN, GPIO_PIN_RESET);
}

void awnings_close(void)
{
    GPIO_WritePin(AWNINGS_GPIO_PORT, AWNINGS_GPIO_PIN, GPIO_PIN_SET);
}

uint8_t awnings_getstate(void)
{
    GPIO_ReadPin(AWNINGS_GPIO_PORT, AWNINGS_GPIO_PIN);
}