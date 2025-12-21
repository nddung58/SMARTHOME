#include "rain.h"
#include "esp_adc/adc_oneshot.h"

#define RAIN_ADC_CH ADC_CHANNEL_4 // GPIO32
#define RAIN_ADC_ATTEN ADC_ATTEN_DB_11
#define ADC_MAX 4095.0f

static adc_oneshot_unit_handle_t rain_adc;

void rain_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&unit_cfg, &rain_adc);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = RAIN_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT};
    adc_oneshot_config_channel(rain_adc, RAIN_ADC_CH, &chan_cfg);
}

float rain_getpercent(void)
{
    int raw = 0;
    adc_oneshot_read(rain_adc, RAIN_ADC_CH, &raw);

    float wet = 1.0f - (raw / ADC_MAX);
    if (wet < 0)
        wet = 0;
    if (wet > 1)
        wet = 1;

    return wet * 100.0f;
}
