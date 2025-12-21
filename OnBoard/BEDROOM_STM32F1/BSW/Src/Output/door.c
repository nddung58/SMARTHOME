#include "door.h"

#include "stm32_gpio.h"
#include "stm32_tim.h"
#include "stdbool.h"

#include "timer_base.h"

static TIM_HandleTypeDef htim1;

#define DOOR_PWM_CHANNEL TIM_CHANNEL_4

#define DOOR_PWM_PORT GPIOA
#define DOOR_PWM_PIN (1U << 11) // PA11

void door_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = DOOR_PWM_PIN,
        .Mode = GPIO_MODE_AF_PP,
        .Speed = GPIO_SPEED_HIGH};

    GPIO_Init(DOOR_PWM_PORT, &gpio);

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 71; // 72Mhz /(71+1) = 1MHz
    htim1.Init.Period = 19999; // 1MHz / (19999+1) = 50Hz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    TIM_PWM_Init(&htim1);

    // 5. PWM channel config
    TIM_OC_InitTypeDef oc = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE};

    TIM_PWM_ConfigChannel(&htim1, &oc, DOOR_PWM_CHANNEL);
    TIM_PWM_Start(&htim1, DOOR_PWM_CHANNEL);
}

static uint16_t angle_to_pulse(uint8_t angle)
{
    if (angle > 180)
        angle = 180;

    // SG90: 0.5 ms → 2.5 ms (500–2500 us)
    float pulse = 500 + (angle / 180.0f) * 2000.0f;

    return (uint16_t)pulse; // trả về số microsecond
}

static bool sg90_set_angle(uint8_t angle)
{
    uint16_t pulse = angle_to_pulse(angle);

    TIM_SetCompare(&htim1, DOOR_PWM_CHANNEL, pulse);

    return true;
}

bool door_open(void)
{
    for (int i = 0; i <= 90; i++)
    {
        if (!sg90_set_angle(i))
        {
            return false;
        }
    }
    return true;
}
bool door_close(void)
{
    for (int i = 90; i >= 0; i--)
    {
        if (!sg90_set_angle(i))
        {
            return false;
        }
    }
    return true;
}
uint8_t door_getstate(void)
{
    uint32_t period = htim1.Init.Period + 1;
    if (period == 0)
        return 0;

    uint16_t pulse = htim1.Instance->CCR4;

    uint16_t pulse_close = angle_to_pulse(0);
    uint16_t pulse_open = angle_to_pulse(90);

    // Ngưỡng ±50 us cho sai số servo
    if (pulse <= pulse_close + 50)
        return 0; // close

    if (pulse >= pulse_open - 50)
        return 1;

    return 0;
}