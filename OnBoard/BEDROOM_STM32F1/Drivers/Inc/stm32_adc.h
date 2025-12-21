/*
 * stm32_adc.h
 *
 *  Created on: May 24, 2025
 *      Author: nguye
 */

/* stm32_adc.h - Simplified ADC Header for STM32F103C6T6 */

#ifndef __STM32_ADC_H
#define __STM32_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal_def.h"

/* ADC Initialization Structure */
typedef struct {
  uint32_t DataAlign;             // ADC_DATAALIGN_RIGHT or ADC_DATAALIGN_LEFT
  uint32_t ScanConvMode;          // ADC_SCAN_ENABLE or ADC_SCAN_DISABLE
  FunctionalState ContinuousConvMode;
  uint32_t NbrOfConversion;       // Number of channels
} ADC_InitTypeDef;

/* ADC Channel Configuration Structure */
typedef struct {
  uint32_t Channel;               // ADC_CHANNEL_X
  uint32_t Rank;                  // ADC_REGULAR_RANK_1, etc.
  uint32_t SamplingTime;          // ADC_SAMPLETIME_XX
} ADC_ChannelConfTypeDef;

/* ADC Handle Structure */
typedef struct __ADC_HandleTypeDef {
  ADC_TypeDef *Instance;          // ADC1 base address
  ADC_InitTypeDef Init;           // ADC configuration
  __IO uint32_t State;            // ADC State
  __IO uint32_t ErrorCode;        // Error Code
} ADC_HandleTypeDef;

/* Basic ADC Macros */
#define ADC_ENABLE(__HANDLE__)   (SET_BIT((__HANDLE__)->Instance->CR2, ADC_CR2_ADON))
#define ADC_DISABLE(__HANDLE__)  (CLEAR_BIT((__HANDLE__)->Instance->CR2, ADC_CR2_ADON))

/* ADC Data Alignment */
#define ADC_DATAALIGN_RIGHT      0x00000000U
#define ADC_DATAALIGN_LEFT       ((uint32_t)ADC_CR2_ALIGN)

/* ADC Scan Mode */
#define ADC_SCAN_DISABLE         0x00000000U
#define ADC_SCAN_ENABLE          ((uint32_t)ADC_CR1_SCAN)

/* ADC Channels (PA0–PA7, PB0, PB1) */
#define ADC_CHANNEL_0            0x00000000U
#define ADC_CHANNEL_1            ((uint32_t)(ADC_SQR3_SQ1_0))
#define ADC_CHANNEL_2            ((uint32_t)(ADC_SQR3_SQ1_1))
#define ADC_CHANNEL_3            ((uint32_t)(ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_0))
#define ADC_CHANNEL_4            ((uint32_t)(ADC_SQR3_SQ1_2))
#define ADC_CHANNEL_5            ((uint32_t)(ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_0))
#define ADC_CHANNEL_6            ((uint32_t)(ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_1))
#define ADC_CHANNEL_7            ((uint32_t)(ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_0))
#define ADC_CHANNEL_8            ((uint32_t)(ADC_SQR3_SQ1_3))
#define ADC_CHANNEL_9            ((uint32_t)(ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_0))

/* ADC Regular Rank */
#define ADC_REGULAR_RANK_1       0x00000001U

/* ADC Sampling Time */
#define ADC_SAMPLETIME_28CYCLES_5 ((uint32_t)(ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0))
#define ADC_SAMPLETIME_71CYCLES_5 ((uint32_t)(ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1))

/* ADC Functions */
StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc);
StatusTypeDef ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
StatusTypeDef ADC_Start(ADC_HandleTypeDef* hadc);
StatusTypeDef ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);
uint32_t ADC_GetValue(ADC_HandleTypeDef* hadc);

/**
 * @brief Đọc tất cả giá trị ADC vào buffer
 * @param hadc: Con trỏ đến cấu trúc ADC_HandleTypeDef
 * @param buffer: Con trỏ đến mảng để lưu giá trị ADC
 * @param count: Số lượng giá trị cần đọc (tối đa 16)
 */
StatusTypeDef ADC_ReadAll(ADC_HandleTypeDef *hadc, uint16_t *buffer, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_ADC_H */
