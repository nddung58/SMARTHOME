#include "stm32_rcc.h"

/**
 * @brief Cấu hình hệ thống clock STM32F1 sử dụng HSE + PLL để chạy ở 72 MHz.
 */
void SystemClock_Config(void)
{
    // 1. Bật HSE
    RCC->CR |= RCC_CR_HSEON;

    // 2. Chờ HSE ổn định
    while (!(RCC->CR & RCC_CR_HSERDY))
        ;

    // 3. Cấu hình Flash: 2 wait states (để hoạt động 72MHz)
    FLASH->ACR |= FLASH_ACR_PRFTBE; // Enable prefetch buffer
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait states

    // 4. Cấu hình PLL: nguồn HSE, nhân 9 => 8MHz x 9 = 72MHz
    RCC->CFGR &= ~RCC_CFGR_PLLSRC; // Clear PLL source
    RCC->CFGR |= RCC_CFGR_PLLSRC;  // PLL source = HSE
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL x9

    // 5. Cấu hình bus
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = HCLK/2 (max 36MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = HCLK

    // 6. Bật PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    // 7. Chọn PLL làm hệ thống clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
}
