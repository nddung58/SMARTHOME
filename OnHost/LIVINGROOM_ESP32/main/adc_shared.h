#ifndef ADC_SHARED_H
#define ADC_SHARED_H

#include "esp_adc/adc_oneshot.h"

extern adc_oneshot_unit_handle_t adc1_handle;

void adc_shared_init(void);

#endif
