#include "sg90.h"
#include "driver/ledc.h"

#define SERVO_GPIO GPIO_NUM_13

void sg90_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel);
}

static uint32_t angle_to_duty_cycle(uint8_t angle)
{
    if (angle > 180)
        angle = 180;

    float pulse = 0.5 + (angle / 180.0) * 2.0; // ms
    float duty_f = (pulse / 20.0) * 4095.0;    // 12-bit resolution
    return (uint32_t)duty_f;
}

bool sg90_set_angle(uint8_t angle)
{
    uint32_t duty = angle_to_duty_cycle(angle);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    return true;
}
