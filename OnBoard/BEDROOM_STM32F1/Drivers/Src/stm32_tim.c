/* stm32_tim.c - Minimal Timer Implementation for STM32F103C6T6 */

#include "stm32_tim.h"

StatusTypeDef TIM_Base_Init(TIM_HandleTypeDef *htim)
{
    if (htim == NULL || htim->Instance == NULL)
        return DUNGX_ERROR;

    // Enable timer clock
    if (htim->Instance == TIM1)
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    else if (htim->Instance == TIM2)
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (htim->Instance == TIM3)
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Cấu hình prescaler và ARR
    htim->Instance->PSC = htim->Init.Prescaler;
    htim->Instance->ARR = htim->Init.Period;

    // Giữ lại các bit khác, chỉ thay thế mode và division
    htim->Instance->CR1 &= ~(TIM_CR1_CMS | TIM_CR1_DIR | TIM_CR1_CKD);
    htim->Instance->CR1 |= htim->Init.CounterMode | htim->Init.ClockDivision;

    return DUNGX_OK;
}

StatusTypeDef TIM_PWM_Init(TIM_HandleTypeDef *htim)
{
    return TIM_Base_Init(htim); // For simplicity, PWM init is same as base init
}

StatusTypeDef TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel)
{
    if (htim == NULL || sConfig == NULL)
        return DUNGX_ERROR;

    // Set mode and polarity
    switch (Channel)
    {
    case TIM_CHANNEL_1:
        htim->Instance->CCMR1 &= ~TIM_CCMR1_OC1M;
        htim->Instance->CCMR1 |= sConfig->OCMode;
        htim->Instance->CCR1 = sConfig->Pulse;

        htim->Instance->CCMR1 |= TIM_CCMR1_OC1PE;
        if (sConfig->OCFastMode == TIM_OCFAST_DISABLE)
            htim->Instance->CCMR1 &= ~TIM_CCMR1_OC1FE;
        if (sConfig->OCPolarity == TIM_OCPOLARITY_LOW)
            htim->Instance->CCER |= TIM_CCER_CC1P;
        else
            htim->Instance->CCER &= ~TIM_CCER_CC1P;
        break;
    case TIM_CHANNEL_2:
        htim->Instance->CCMR1 &= ~TIM_CCMR1_OC2M;
        htim->Instance->CCMR1 |= (sConfig->OCMode << 8);
        htim->Instance->CCR2 = sConfig->Pulse;
        if (sConfig->OCPolarity == TIM_OCPOLARITY_LOW)
            htim->Instance->CCER |= TIM_CCER_CC2P;
        else
            htim->Instance->CCER &= ~TIM_CCER_CC2P;
        break;
    case TIM_CHANNEL_3:
        htim->Instance->CCMR2 &= ~TIM_CCMR2_OC3M;
        htim->Instance->CCMR2 |= sConfig->OCMode;
        htim->Instance->CCR3 = sConfig->Pulse;
        if (sConfig->OCPolarity == TIM_OCPOLARITY_LOW)
            htim->Instance->CCER |= TIM_CCER_CC3P;
        else
            htim->Instance->CCER &= ~TIM_CCER_CC3P;
        break;
    case TIM_CHANNEL_4:
        htim->Instance->CCMR2 &= ~TIM_CCMR2_OC4M;
        htim->Instance->CCMR2 |= (sConfig->OCMode << 8);
        htim->Instance->CCR4 = sConfig->Pulse;
        if (sConfig->OCPolarity == TIM_OCPOLARITY_LOW)
            htim->Instance->CCER |= TIM_CCER_CC4P;
        else
            htim->Instance->CCER &= ~TIM_CCER_CC4P;
        break;
    default:
        return DUNGX_ERROR;
    }

    return DUNGX_OK;
}

StatusTypeDef TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    switch (Channel)
    {
    case TIM_CHANNEL_1:
        htim->Instance->CCER |= TIM_CCER_CC1E;
        break;
    case TIM_CHANNEL_2:
        htim->Instance->CCER |= TIM_CCER_CC2E;
        break;
    case TIM_CHANNEL_3:
        htim->Instance->CCER |= TIM_CCER_CC3E;
        break;
    case TIM_CHANNEL_4:
        htim->Instance->CCER |= TIM_CCER_CC4E;
        break;
    default:
        return DUNGX_ERROR;
    }

    if (htim->Instance == TIM1)
    {
        htim->Instance->BDTR |= TIM_BDTR_MOE;
        htim->Instance->CR1 |= TIM_CR1_ARPE; // Auto-reload preload enable
    }

    TIM_ENABLE(htim);
    return DUNGX_OK;
}

StatusTypeDef TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    switch (Channel)
    {
    case TIM_CHANNEL_1:
        htim->Instance->CCER &= ~TIM_CCER_CC1E;
        break;
    case TIM_CHANNEL_2:
        htim->Instance->CCER &= ~TIM_CCER_CC2E;
        break;
    case TIM_CHANNEL_3:
        htim->Instance->CCER &= ~TIM_CCER_CC3E;
        break;
    case TIM_CHANNEL_4:
        htim->Instance->CCER &= ~TIM_CCER_CC4E;
        break;
    default:
        return DUNGX_ERROR;
    }
    TIM_DISABLE(htim);
    return DUNGX_OK;
}

void TIM_SetCompare(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t Compare)
{
    if (htim == NULL)
        return;

    switch (Channel)
    {
    case TIM_CHANNEL_1:
        htim->Instance->CCR1 = Compare;
        break;
    case TIM_CHANNEL_2:
        htim->Instance->CCR2 = Compare;
        break;
    case TIM_CHANNEL_3:
        htim->Instance->CCR3 = Compare;
        break;
    case TIM_CHANNEL_4:
        htim->Instance->CCR4 = Compare;
        break;
    default:
        break;
    }
}
