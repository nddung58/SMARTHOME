/* stm32_gpio.h - Minimal GPIO HAL Interface for STM32F103C6T6 with EXTI */

#ifndef __STM32_GPIO_H
#define __STM32_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal_def.h"

/* GPIO Pin definition */
#define GPIO_PIN_0      ((uint16_t)0x0001)
#define GPIO_PIN_1      ((uint16_t)0x0002)
#define GPIO_PIN_2      ((uint16_t)0x0004)
#define GPIO_PIN_3      ((uint16_t)0x0008)
#define GPIO_PIN_4      ((uint16_t)0x0010)
#define GPIO_PIN_5      ((uint16_t)0x0020)
#define GPIO_PIN_6      ((uint16_t)0x0040)
#define GPIO_PIN_7      ((uint16_t)0x0080)
#define GPIO_PIN_8      ((uint16_t)0x0100)
#define GPIO_PIN_9      ((uint16_t)0x0200)
#define GPIO_PIN_10     ((uint16_t)0x0400)
#define GPIO_PIN_11     ((uint16_t)0x0800)
#define GPIO_PIN_12     ((uint16_t)0x1000)
#define GPIO_PIN_13     ((uint16_t)0x2000)
#define GPIO_PIN_14     ((uint16_t)0x4000)
#define GPIO_PIN_15     ((uint16_t)0x8000)
#define GPIO_PIN_ALL    ((uint16_t)0xFFFF)

/* GPIO Mode */
#define  GPIO_MODE_INPUT                        0x00000000u   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    0x00000001u   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    0x00000011u   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        0x00000002u   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        0x00000012u   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_AF_INPUT                     GPIO_MODE_INPUT          /*!< Alternate Function Input Mode         */

#define  GPIO_MODE_ANALOG                       0x00000003u   /*!< Analog Mode  */

#define EXTI_MODE            0x10000000u
#define RISING_EDGE          0x00100000u
#define FALLING_EDGE         0x00200000u

#define GPIO_MODE_IT_RISING          (EXTI_MODE | RISING_EDGE)
#define GPIO_MODE_IT_FALLING         (EXTI_MODE | FALLING_EDGE)
#define GPIO_MODE_IT_RISING_FALLING  (EXTI_MODE | RISING_EDGE | FALLING_EDGE)

#define GPIO_MODE_EVT_RISING          = 0x20000000 | RISING_EDGE
#define GPIO_MODE_EVT_FALLING         = 0x20000000 | FALLING_EDGE
#define GPIO_MODE_EVT_RISING_FALLING  = 0x20000000 | RISING_EDGE | FALLING_EDGE

/* GPIO Pull */
#define GPIO_NOPULL            0x00u
#define GPIO_PULLUP            0x01u
#define GPIO_PULLDOWN          0x02u

/* GPIO Speed */
#define GPIO_SPEED_LOW         0x00u
#define GPIO_SPEED_MEDIUM      0x01u
#define GPIO_SPEED_HIGH        0x02u

/* GPIO Pin state */
typedef enum {
  GPIO_PIN_RESET = 0u,
  GPIO_PIN_SET = 1u
} GPIO_PinState;

/* GPIO Initialization Structure */
typedef struct {
  uint32_t Pin;     // GPIO_PIN_X
  uint32_t Mode;    // GPIO_MODE_OUTPUT_PP, etc.
  uint32_t Pull;    // GPIO_PULLUP, etc.
  uint32_t Speed;   // GPIO_SPEED_HIGH, etc.
} GPIO_InitTypeDef;

/* Function Prototypes */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* EXTI Functions */
void GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_GPIO_H */
