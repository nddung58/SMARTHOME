#include "ldr.h"
#include "adc_shared.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

#define LDR_ADC_CHANNEL ADC_CHANNEL_6
#define LDR_ADC_ATTEN ADC_ATTEN_DB_11

static adc_cali_handle_t ldr_cali = NULL;
static bool ldr_has_cali = false;

void ldr_init(void)
{
    adc_shared_init();

    adc_oneshot_chan_cfg_t cfg = {
        .atten = LDR_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT};
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, LDR_ADC_CHANNEL, &cfg));

    adc_cali_line_fitting_config_t c = {
        .unit_id = ADC_UNIT_1,
        .atten = LDR_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT};
    ldr_has_cali = (adc_cali_create_scheme_line_fitting(&c, &ldr_cali) == ESP_OK);
}

static float ldr_voltage(void)
{
    int raw = 0, mv = 0;

    adc_oneshot_read(adc1_handle, LDR_ADC_CHANNEL, &raw);

    if (ldr_has_cali)
        adc_cali_raw_to_voltage(ldr_cali, raw, &mv);
    else
        mv = (raw * 3300) / 4095;

    return mv / 1000.0f;
}

float ldr_getlux(void)
{
    float v = ldr_voltage();

    float light = 1.0f - v / 3.3f;
    if (light < 0)
        light = 0;
    if (light > 1)
        light = 1;

    if (light < 0.2f)
        return light * 100;
    if (light < 0.7f)
        return 100 + (light - 0.2f) * 800;
    return 500 + (light - 0.7f) * 20000;
}
