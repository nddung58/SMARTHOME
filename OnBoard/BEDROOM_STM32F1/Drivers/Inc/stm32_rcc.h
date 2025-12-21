/* stm32_rcc.h - Minimal GPIO Interface for STM32F103C6T6 with EXTI */

#ifndef __STM32_RCC_H
#define __STM32_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal_def.h"

void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_RCC_H */