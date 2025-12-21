#include "mq2.h"
#include "adc_shared.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define MQ2_ADC_CH ADC_CHANNEL_7
#define MQ2_ADC_ATTEN ADC_ATTEN_DB_11

static adc_cali_handle_t mq2_cali = NULL;
static bool mq2_has_cali = false;

void mq2_init(void)
{
    adc_shared_init(); // dùng chung handle

    adc_oneshot_chan_cfg_t ch_cfg = {
        .atten = MQ2_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, MQ2_ADC_CH, &ch_cfg));

    // Calibration
    adc_cali_line_fitting_config_t lc = {
        .unit_id = ADC_UNIT_1,
        .atten = MQ2_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    mq2_has_cali = (adc_cali_create_scheme_line_fitting(&lc, &mq2_cali) == ESP_OK);
}

static float mq2_raw_voltage(void)
{
    int raw = 0, mv = 0;

    adc_oneshot_read(adc1_handle, MQ2_ADC_CH, &raw);

    if (mq2_has_cali)
        adc_cali_raw_to_voltage(mq2_cali, raw, &mv);
    else
        mv = (raw * 3300) / 4095;

    return mv / 1000.0f;
}

float mq2_getppm(void)
{
    float v = mq2_raw_voltage();
    return (v / 3.3f) * 2000.0f;
}
