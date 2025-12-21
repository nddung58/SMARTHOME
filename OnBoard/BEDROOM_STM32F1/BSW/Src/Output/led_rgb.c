#include "../../../BSW/Inc/Output/led_rgb.h"

#include "stm32_gpio.h"
#include "stm32_tim.h"

#define LED_R_PORT GPIOA
#define LED_R_PIN (1U << 6) // PA6 - TIM3_CH1

#define LED_G_PORT GPIOA
#define LED_G_PIN (1U << 7) // PA7 - TIM3_CH2

#define LED_B_PORT GPIOB
#define LED_B_PIN (1U << 0) // PB0 - TIM3_CH3

static TIM_HandleTypeDef htim3;

static LED_State_t current_led_state = LED_OFF;

const RGB_Color_t led_color_table[] = {
    [LED_OFF] = {0, 0, 0},
    [LED_WHITE_50] = {127, 127, 127},
    [LED_WHITE_100] = {255, 255, 255},
    [LED_GREEN] = {0, 255, 0},
    [LED_RED] = {255, 0, 0}};

void LED_RGB_Init(void)
{
    // 1. Cấu hình các chân PWM: PA6, PA7, PB0 (AF_PP)
    GPIO_InitTypeDef gpio;

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_HIGH;

    gpio.Pin = LED_R_PIN;
    GPIO_Init(LED_R_PORT, &gpio);

    gpio.Pin = LED_G_PIN;
    GPIO_Init(LED_G_PORT, &gpio);

    gpio.Pin = LED_B_PIN;
    GPIO_Init(LED_B_PORT, &gpio);

    // 2. Cấu hình timer TIM3
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71; // 72 MHz / (71+1) = 1 MHz
    htim3.Init.Period = 999;   // PWM tần số = 1 kHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM_PWM_Init(&htim3);

    // 3. Cấu hình từng kênh PWM (CH1–CH3)
    TIM_OC_InitTypeDef sConfigOC = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE};

    TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1); // RED
    TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2); // GREEN
    TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3); // BLUE

    TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}

void LED_RGB_Set(uint8_t r_percent, uint8_t g_percent, uint8_t b_percent)
{
    uint32_t period = htim3.Init.Period + 1;

    TIM_SetCompare(&htim3, TIM_CHANNEL_1, (r_percent * period) / 255);
    TIM_SetCompare(&htim3, TIM_CHANNEL_2, (g_percent * period) / 255);
    TIM_SetCompare(&htim3, TIM_CHANNEL_3, (b_percent * period) / 255);
}

void LED_RGB_Off(void)
{
    TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
    TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
    TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
}

void LED_RGB_Get(uint8_t *r_percent, uint8_t *g_percent, uint8_t *b_percent)
{
    uint32_t period = htim3.Init.Period + 1;

    *r_percent = (htim3.Instance->CCR1 * 255) / period;
    *g_percent = (htim3.Instance->CCR2 * 255) / period;
    *b_percent = (htim3.Instance->CCR3 * 255) / period;
}

void LED_RGB_SetState(LED_State_t state)
{
    if (state < LED_OFF || state > LED_RED)
    {
        return; // Invalid state
    }

    current_led_state = state;

    RGB_Color_t color = led_color_table[state];
    LED_RGB_Set(color.R, color.G, color.B);
}

LED_State_t LED_RGB_GetState(void)
{
    uint8_t r, g, b;
    LED_RGB_Get(&r, &g, &b);

    for (LED_State_t state = LED_OFF; state <= LED_RED; state++)
    {
        RGB_Color_t color = led_color_table[state];
        if (r == color.R && g == color.G && b == color.B)
        {
            return state;
        }
    }

    return LED_UNKNOWN;
}