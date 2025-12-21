/* stm32_adc.c - Minimal ADC Driver for STM32F103C6T6 */

#include "stm32_adc.h"

StatusTypeDef ADC_Init(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL || hadc->Instance == NULL)
    {
        return DUNGX_ERROR;
    }

    // 0. Đảm bảo tần số ADC < 14 MHz bằng cách chia prescaler
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;  // PCLK2 / 6 = 12 MHz

    // 1. Enable ADCx Clock (tùy theo Instance)
    if (hadc->Instance == ADC1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    }
    else if (hadc->Instance == ADC2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    }
#ifdef RCC_APB2ENR_ADC3EN
    else if (hadc->Instance == ADC3)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    }
#endif
    else
    {
        return DUNGX_ERROR;
    }

    // Reset CR1 and CR2
    hadc->Instance->CR1 = 0;
    hadc->Instance->CR2 = 0;

    // Set scan mode
    if (hadc->Init.ScanConvMode == ADC_SCAN_ENABLE)
    {
        hadc->Instance->CR1 |= ADC_SCAN_ENABLE;
    }

    // Set data alignment
    if (hadc->Init.DataAlign == ADC_DATAALIGN_LEFT)
    {
        hadc->Instance->CR2 |= ADC_DATAALIGN_LEFT;
    }

    // Set continuous conversion
    if (hadc->Init.ContinuousConvMode == ENABLE)
    {
        hadc->Instance->CR2 |= ADC_CR2_CONT;
    }

    // Set number of conversions
    hadc->Instance->SQR1 &= ~ADC_SQR1_L;
    hadc->Instance->SQR1 |= (hadc->Init.NbrOfConversion - 1) << 20;

    // Enable ADC
    ADC_ENABLE(hadc);

    // Short delay before calibration
    for (volatile int i = 0; i < 1000; ++i)
        ;

    // Calibration
    hadc->Instance->CR2 |= ADC_CR2_CAL;
    while (hadc->Instance->CR2 & ADC_CR2_CAL)
        ;

    return DUNGX_OK;
}

StatusTypeDef ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    if (hadc == NULL || sConfig == NULL)
    {
        return DUNGX_ERROR;
    }

    uint8_t channel = sConfig->Channel;
    uint8_t rank = sConfig->Rank;

    if (rank < 1 || rank > 16)
        return DUNGX_ERROR; // Rank chỉ cho phép từ 1–16

    // 1. Gán kênh vào đúng vị trí trong dãy SQRx
    if (rank <= 6)
    {
        // SQR3 chứa rank 1–6
        hadc->Instance->SQR3 &= ~(0x1F << ((rank - 1) * 5));
        hadc->Instance->SQR3 |=  (channel & 0x1F) << ((rank - 1) * 5);
    }
    else if (rank <= 12)
    {
        // SQR2 chứa rank 7–12
        hadc->Instance->SQR2 &= ~(0x1F << ((rank - 7) * 5));
        hadc->Instance->SQR2 |=  (channel & 0x1F) << ((rank - 7) * 5);
    }
    else
    {
        // SQR1 chứa rank 13–16
        hadc->Instance->SQR1 &= ~(0x1F << ((rank - 13) * 5));
        hadc->Instance->SQR1 |=  (channel & 0x1F) << ((rank - 13) * 5);
    }

    // --- 2. Cấu hình thời gian lấy mẫu ---
    if (channel <= 9)
    {
        // SMPR2 dùng cho kênh 0–9
        hadc->Instance->SMPR2 &= ~(0x7 << (3 * channel));
        hadc->Instance->SMPR2 |= (sConfig->SamplingTime << (3 * channel));
    }
    else if (channel <= 17)
    {
        // SMPR1 dùng cho kênh 10–17
        uint8_t ch_index = channel - 10;
        hadc->Instance->SMPR1 &= ~(0x7 << (3 * ch_index));
        hadc->Instance->SMPR1 |= (sConfig->SamplingTime << (3 * ch_index));
    }
    else
    {
        return DUNGX_ERROR; 
    }

    return DUNGX_OK;
}


StatusTypeDef ADC_Start(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL)
        return DUNGX_ERROR;

    uint8_t conversions = hadc->Init.NbrOfConversion;
    if (conversions < 1 || conversions > 16)
        return DUNGX_ERROR;

    // Enable ADC (1st time)
    hadc->Instance->CR2 |= ADC_CR2_ADON;
    for (volatile int i = 0; i < 1000; ++i); // delay nhỏ

    // Enable ADC (2nd time) – start conversion ready
    hadc->Instance->CR2 |= ADC_CR2_ADON;

    // If using software trigger (default), need SWSTART
    if ((hadc->Init.ContinuousConvMode == DISABLE) &&
        (hadc->Init.ScanConvMode == ADC_SCAN_ENABLE || conversions > 1))
    {
        hadc->Instance->CR2 |= ADC_CR2_SWSTART;
    }

    return DUNGX_OK;
}


StatusTypeDef ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout)
{
    if (hadc == NULL)
    {
        return DUNGX_ERROR;
    }

    while (!(hadc->Instance->SR & ADC_SR_EOC))
    {
        if (Timeout-- == 0)
            return DUNGX_TIMEOUT;
    }

    return DUNGX_OK;
}

uint32_t ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL)
        return 0;
    return hadc->Instance->DR;
}

StatusTypeDef ADC_ReadAll(ADC_HandleTypeDef *hadc, uint16_t *buffer, uint8_t count)
{
    if (hadc == NULL || buffer == NULL || count == 0)
        return DUNGX_ERROR;

    ADC_Start(hadc);

    for (uint8_t i = 0; i < count; i++)
    {
        if (ADC_PollForConversion(hadc, 1000) != DUNGX_OK)
            return DUNGX_TIMEOUT;

        buffer[i] = (uint16_t)ADC_GetValue(hadc);
        hadc->Instance->SR &= ~ADC_SR_EOC; // clear EOC flag
    }

    return DUNGX_OK;
}

