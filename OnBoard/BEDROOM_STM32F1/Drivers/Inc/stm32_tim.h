/* stm32_tim.h - Minimal Timer HAL Interface for STM32F103C6T6 */

#ifndef __STM32_TIM_H
#define __STM32_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal_def.h"

#define SystemCoreClock 72000000

/* Timer base initialization structure */
typedef struct {
  uint32_t Prescaler;        // Clock prescaler
  uint32_t CounterMode;      // TIM_COUNTERMODE_UP or TIM_COUNTERMODE_DOWN
  uint32_t Period;           // Auto-reload value
  uint32_t ClockDivision;    // TIM_CLOCKDIVISION_DIV1 usually
} TIM_Base_InitTypeDef;

/* Timer output compare (PWM) configuration */
typedef struct {
  uint32_t OCMode;           // TIM_OCMODE_PWM1
  uint32_t Pulse;            // Duty cycle (0 to Period)
  uint32_t OCPolarity;       // TIM_OCPOLARITY_HIGH or LOW
  uint32_t OCFastMode;       // TIM_OCFAST_DISABLE
} TIM_OC_InitTypeDef;

typedef enum
{
  HAL_TIM_STATE_RESET             = 0x00U,    /*!< Peripheral not yet initialized or disabled  */
  HAL_TIM_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use    */
  HAL_TIM_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing              */
  HAL_TIM_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                               */
  HAL_TIM_STATE_ERROR             = 0x04U     /*!< Reception process is ongoing                */
} TIM_StateTypeDef;

/* Timer handler structure */
typedef struct __TIM_HandleTypeDef {
  TIM_TypeDef *Instance;             // TIMx instance
  TIM_Base_InitTypeDef Init;        // Basic timer config
  __IO HAL_LockTypeDef Lock;        // Lock state
  __IO TIM_StateTypeDef State;  // HAL state
} TIM_HandleTypeDef;

/* Exported macros */
#define TIM_ENABLE(__HANDLE__)     ((__HANDLE__)->Instance->CR1 |= TIM_CR1_CEN)
#define TIM_DISABLE(__HANDLE__)    ((__HANDLE__)->Instance->CR1 &= ~TIM_CR1_CEN)

/* Counter modes */
#define TIM_COUNTERMODE_UP               ((uint32_t)0x00000000U)
#define TIM_COUNTERMODE_DOWN             ((uint32_t)TIM_CR1_DIR)

/* Clock division */
#define TIM_CLOCKDIVISION_DIV1           ((uint32_t)0x00000000U)

/* Output Compare Modes */
#define TIM_OCMODE_PWM1                  ((uint32_t)0x00000006U) << 4U

/* Output Compare Polarity */
#define TIM_OCPOLARITY_HIGH              ((uint32_t)0x00000000U)
#define TIM_OCPOLARITY_LOW               ((uint32_t)TIM_CCER_CC1P)

/* Output Compare Fast Mode */
#define TIM_OCFAST_DISABLE               ((uint32_t)0x00000000U)

/* Channel definitions */
#define TIM_CHANNEL_1                    0x00000000U
#define TIM_CHANNEL_2                    0x00000004U
#define TIM_CHANNEL_3                    0x00000008U
#define TIM_CHANNEL_4                    0x0000000CU

void TIM_SetCompare(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t Compare);

/* Timer HAL functions */
StatusTypeDef TIM_Base_Init(TIM_HandleTypeDef *htim);
StatusTypeDef TIM_PWM_Init(TIM_HandleTypeDef *htim);
StatusTypeDef TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel);
StatusTypeDef TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
StatusTypeDef TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_TIM_H */
