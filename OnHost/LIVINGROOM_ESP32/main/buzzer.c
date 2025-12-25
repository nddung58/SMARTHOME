#include "buzzer.h"
#include "output.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZER_PIN GPIO_NUM_5
#define duration_time_beep_ms 1000

void buzzer_init(void)
{
    output_io_init(BUZZER_PIN);
    buzzer_off();
}

void buzzer_on(void)
{
    output_io_set_level(BUZZER_PIN, 0);
}
void buzzer_off(void)
{
    output_io_set_level(BUZZER_PIN, 1);
}
void buzzer_beep(void)
{
    output_io_set_level(BUZZER_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(duration_time_beep_ms));
    output_io_set_level(BUZZER_PIN, 1);
}