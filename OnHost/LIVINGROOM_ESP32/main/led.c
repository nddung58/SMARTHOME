#include "led.h"
#include "driver/ledc.h"

#define LED_R_PIN 12
#define LED_G_PIN 14
#define LED_B_PIN 27

const RGB_Color_t led_color_table[] = {
    [LED_OFF] = {0, 0, 0},
    [LED_WHITE_50] = {127, 127, 127},
    [LED_WHITE_100] = {255, 255, 255},
    [LED_WARM] = {255, 214, 170},
    [LED_RED] = {255, 0, 0}};

void led_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT, // 0–255
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channels[3] = {
        {.channel = LEDC_CHANNEL_0,
         .duty = 0,
         .gpio_num = LED_R_PIN,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_TIMER_0},
        {.channel = LEDC_CHANNEL_1,
         .duty = 0,
         .gpio_num = LED_G_PIN,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_TIMER_0},
        {.channel = LEDC_CHANNEL_2,
         .duty = 0,
         .gpio_num = LED_B_PIN,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_TIMER_0}};

    for (int i = 0; i < 3; i++)
        ledc_channel_config(&ledc_channels[i]);
}

static void rgb_set(uint8_t r, uint8_t g, uint8_t b)
{
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, r);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, g);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, b);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
}

void led_set(LED_State_t state)
{
    if (state < LED_OFF || state > LED_RED)
    {
        return; // Invalid state
    }

    const RGB_Color_t color = led_color_table[state];
    rgb_set(color.R, color.G, color.B);
}

RGB_Color_t led_get_color(void)
{
    RGB_Color_t color;

    color.R = ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    color.G = ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    color.B = ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

    return color;
}

LED_State_t led_getstatus(void)
{
    RGB_Color_t hw = led_get_color();

    for (int i = 0; i < LED_UNKNOWN; i++)
    {
        RGB_Color_t c = led_color_table[i];

        if (hw.R == c.R && hw.G == c.G && hw.B == c.B)
        {
            return (LED_State_t)i;
        }
    }

    return LED_OFF;
}