#include "../../../BSW/Inc/Input/mq2.h"

#include "stm32_adc.h"
#include "stm32_gpio.h"

#define MQ2_PORT GPIOA
#define MQ2_PIN (1U << 1)             // PA1
#define MQ2_ADC_CHANNEL ADC_CHANNEL_1 // ADC1_IN1
#define MQ2_ADC_SAMPLING ADC_SAMPLETIME_71CYCLES_5

#define ADC_MAX 4095.0f // 12-bit ADC

static ADC_HandleTypeDef hadc_mq2;

void MQ2_Init(void)
{
    // Cấu hình chân analog PA1
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = MQ2_PIN,
        .Mode = GPIO_MODE_ANALOG};
    GPIO_Init(MQ2_PORT, &GPIO_InitStruct);

    // Cấu hình ADC
    hadc_mq2.Instance = ADC2;
    hadc_mq2.Init.ContinuousConvMode = DISABLE;
    hadc_mq2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc_mq2.Init.NbrOfConversion = 1;
    hadc_mq2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    ADC_Init(&hadc_mq2);

    ADC_ChannelConfTypeDef sConfig = {
        .Channel = 1, // ADC1_IN1 (PA1)
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = MQ2_ADC_SAMPLING};
    ADC_ConfigChannel(&hadc_mq2, &sConfig);
}

/**
 * @brief Đọc mức khí từ MQ2 (giá trị từ 0 đến 2000)
 * @return uint16_t mức khí (ppm giả định hoặc chất lượng)
 */
float MQ2_ReadLevel(void)
{
    ADC_Start(&hadc_mq2);
    if (ADC_PollForConversion(&hadc_mq2, 1000) != DUNGX_OK)
        return 0;
    float raw_adc = (float)ADC_GetValue(&hadc_mq2);

    return raw_adc * 2000.0f / ADC_MAX;
}

/**
 * @brief Đọc giá trị ADC từ MQ2
 * @return uint16_t giá trị ADC (0-4095)
 */
uint16_t MQ2_ReadRaw(void)
{
    ADC_Start(&hadc_mq2);
    if (ADC_PollForConversion(&hadc_mq2, 1000) != DUNGX_OK)
        return 0;
    return ADC_GetValue(&hadc_mq2);
}
