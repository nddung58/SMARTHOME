#include "adc_shared.h"
#include "esp_adc/adc_oneshot.h"

adc_oneshot_unit_handle_t adc1_handle = NULL;

void adc_shared_init(void)
{
    if (adc1_handle == NULL)
    {
        adc_oneshot_unit_init_cfg_t cfg = {
            .unit_id = ADC_UNIT_1,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&cfg, &adc1_handle));
    }
}
