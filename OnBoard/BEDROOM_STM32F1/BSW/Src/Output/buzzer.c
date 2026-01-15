/*
 * siren.c
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#include <buzzer.h>
#include "stm32_gpio.h"
#include "timer_base.h"

#define BUZZER_GPIO_PORT GPIOA
#define BUZZER_GPIO_PIN (1U << 3) // PA3

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = BUZZER_GPIO_PIN,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_HIGH};
    GPIO_Init(BUZZER_GPIO_PORT, &gpio);
    GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET);
}

void Buzzer_On(void)
{
    GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET);
}

void Buzzer_Toggle(void)
{
    GPIO_TogglePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

void Buzzer_Beep(uint32_t duration_ms)
{
    Siren_On();
    Delay_ms(duration_ms);
    Siren_Off();
}

uint8_t Buzzer_GetState(void)
{
    GPIO_PinState state = GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);

    // Còi kêu khi pin ở mức RESET (0)
    return (state == GPIO_PIN_RESET) ? 1 : 0;
}
