#include "rain.h"

#include "stm32_gpio.h"

#define RAIN_GPIO_PORT GPIOA
#define RAIN_GPIO_PIN (1U << 4) // PA4

void rain_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = RAIN_GPIO_PIN,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH};
    GPIO_Init(RAIN_GPIO_PORT, &gpio);
    GPIO_WritePin(RAIN_GPIO_PORT, RAIN_GPIO_PIN, GPIO_PIN_SET);
}

uint8_t rain_getpercent(void)
{
    GPIO_PinState state = GPIO_ReadPin(RAIN_GPIO_PORT, RAIN_GPIO_PIN);

    return (state == GPIO_PIN_RESET) ? 73 : 20;
}
