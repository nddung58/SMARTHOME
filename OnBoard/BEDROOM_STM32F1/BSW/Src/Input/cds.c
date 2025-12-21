/*
* cds.c
*
*  Created on: May 22, 2025
*      Author: nguye
*/


#include "../../../BSW/Inc/Input/cds.h"

#include "stm32_gpio.h"
#include "stm32_adc.h"

// === Hardware Definitions ===
#define CDS_GPIO_PORT GPIOA
#define CDS_GPIO_PIN (1U << 0)        // PA0
#define CDS_ADC_CHANNEL ADC_CHANNEL_0 // ADC1_IN0
#define CDS_ADC_SAMPLING ADC_SAMPLETIME_71CYCLES_5

static ADC_HandleTypeDef hadc_cds;

void CDS_Init(void)
{
   // 1. Configure PA0 as analog input
   GPIO_InitTypeDef gpio_init = {
       .Pin = CDS_GPIO_PIN,
       .Mode = GPIO_MODE_ANALOG};
   GPIO_Init(CDS_GPIO_PORT, &gpio_init);

   // 2. Configure ADC1
   hadc_cds.Instance = ADC1;
   hadc_cds.Init.DataAlign = ADC_DATAALIGN_RIGHT;
   hadc_cds.Init.ScanConvMode = ADC_SCAN_DISABLE;
   hadc_cds.Init.ContinuousConvMode = DISABLE;
   hadc_cds.Init.NbrOfConversion = 1;
   ADC_Init(&hadc_cds);

   // 3. Configure ADC channel (Channel 0 = PA0)
   ADC_ChannelConfTypeDef ch_config = {
       .Channel = CDS_ADC_CHANNEL,
       .Rank = ADC_REGULAR_RANK_1,
       .SamplingTime = CDS_ADC_SAMPLING};
   ADC_ConfigChannel(&hadc_cds, &ch_config);
}

uint16_t CDS_ReadRaw(void)
{
   ADC_Start(&hadc_cds);
   if (ADC_PollForConversion(&hadc_cds, 1000) != DUNGX_OK)
       return 0; // Timeout or error
   return (uint16_t)ADC_GetValue(&hadc_cds);
}

float CDS_ReadLux(void)
{
   uint16_t raw = CDS_ReadRaw();

   // Convert raw ADC value to lux
   float lux = (float)(4095 - raw) * 2000.0f / 4095.0f;
   return lux;
}
