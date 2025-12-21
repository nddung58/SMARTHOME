/* stm32_gpio.c - Minimal GPIO Implementation for STM32F103C6T6 with EXTI */

#include "stm32_gpio.h"

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t pos = 0;

    if (GPIOx == GPIOA) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    } else if (GPIOx == GPIOB) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    } else if (GPIOx == GPIOC) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }

    while ((GPIO_Init->Pin >> pos) != 0x00u)
    {
        if ((GPIO_Init->Pin & (1u << pos)) != 0)
        {
            uint32_t shift = (pos % 8) * 4;
            __IO uint32_t *reg = (pos < 8) ? &GPIOx->CRL : &GPIOx->CRH;
            uint32_t mode = GPIO_Init->Mode;
            uint8_t is_exti = ((mode & EXTI_MODE) == EXTI_MODE);

            *reg &= ~(0xFu << shift);

            /* INPUT (thường) + INPUT AF + EXTI đều cấu hình thành input */
            if (mode == GPIO_MODE_INPUT || mode == GPIO_MODE_AF_INPUT || is_exti)
            {
                if (GPIO_Init->Pull == GPIO_PULLUP)
                {
                    *reg |= (0x08u << shift);     // CNF = 10 (pull-up/down), MODE = 00
                    GPIOx->BSRR = (1u << pos);
                }
                else if (GPIO_Init->Pull == GPIO_PULLDOWN)
                {
                    *reg |= (0x08u << shift);     // CNF = 10 (pull-up/down), MODE = 00
                    GPIOx->BRR = (1u << pos);
                }
                else
                {
                    *reg |= (0x04u << shift);     // CNF = 01 (floating), MODE = 00
                }
            }
            else if (mode == GPIO_MODE_ANALOG)
            {
                // CNF = 00, MODE = 00 => đã clear ở trên
            }
            else    /* Output hoặc AF output */
            {
                uint32_t mode_bits = 0x00u;
                if (GPIO_Init->Speed == GPIO_SPEED_LOW)      mode_bits = 0x02;
                else if (GPIO_Init->Speed == GPIO_SPEED_MEDIUM) mode_bits = 0x01;
                else                                         mode_bits = 0x03;

                uint32_t cnf = 0;
                if (mode == GPIO_MODE_OUTPUT_PP)      cnf = 0x00;
                else if (mode == GPIO_MODE_OUTPUT_OD) cnf = 0x04;
                else if (mode == GPIO_MODE_AF_PP)     cnf = 0x08;
                else if (mode == GPIO_MODE_AF_OD)     cnf = 0x0C;

                *reg |= ((mode_bits | cnf) << shift);
            }

            if (is_exti)
            {
                RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

                uint32_t exti_line = 1u << pos;

                AFIO->EXTICR[pos >> 2] &= ~(0xFu << (4 * (pos & 0x03)));
                AFIO->EXTICR[pos >> 2] |= ((GPIOx == GPIOA ? 0 : (GPIOx == GPIOB ? 1 : 2)) << (4 * (pos & 0x03)));

                if (mode & RISING_EDGE)  EXTI->RTSR |= exti_line; else EXTI->RTSR &= ~exti_line;
                if (mode & FALLING_EDGE) EXTI->FTSR |= exti_line; else EXTI->FTSR &= ~exti_line;

                EXTI->IMR |= exti_line;
            }
        }
        pos++;
    }
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (PinState != GPIO_PIN_RESET)
        GPIOx->BSRR = GPIO_Pin;
    else
        GPIOx->BRR = GPIO_Pin;
}

GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    return (GPIOx->IDR & GPIO_Pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;
}

void GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
    if (EXTI->PR & GPIO_Pin)
    {
        EXTI->PR = GPIO_Pin;
        GPIO_EXTI_Callback(GPIO_Pin);
    }
}

__weak void GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // Override in up layer
}
