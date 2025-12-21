#include "pir.h"
#include "stm32_gpio.h"
#include "timer_base.h"
#include "stdint.h"

#define PIR_GPIO_PORT GPIOA
#define PIR_GPIO_PIN  GPIO_PIN_5

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_13

#define PIR_LOCK_MS      2000    // chỉ cho phép trigger 1 lần mỗi 2s
#define PIR_LED_ON_MS    2000    // LED sáng 2s

volatile uint8_t pir_trigger = 0;
volatile uint32_t pir_last_trigger = 0;

static uint8_t led_on = 0;
static uint32_t led_off_time = 0;

/* ======================= INIT =========================== */

void pir_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin  = PIR_GPIO_PIN,
        .Mode = GPIO_MODE_IT_RISING,
        .Pull = GPIO_NOPULL,
    };
    GPIO_Init(PIR_GPIO_PORT, &gpio);

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 2);
}

void pir_test_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_LOW
    };
    GPIO_Init(LED_PORT, &gpio);

    GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
}

/* ================== INTERRUPT CALLBACK ================== */

void GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PIR_GPIO_PIN)
    {
        uint32_t now = GetTick();

        // Chỉ cho phép 1 lần trigger trong PIR_LOCK_MS
        if (now - pir_last_trigger < PIR_LOCK_MS)
            return;

        pir_last_trigger = now;
        pir_trigger = 1;
    }
}

void EXTI9_5_IRQHandler(void)
{
    GPIO_EXTI_IRQHandler(PIR_GPIO_PIN);
}

/* ====================== MAIN PROCESS ====================== */

void pir_process(void)
{
    uint32_t now = GetTick();

    if (pir_trigger)
    {
        pir_trigger = 0;

        // bắt đầu bật LED không chặn CPU
        led_on = 1;
        led_off_time = now + PIR_LED_ON_MS;
        GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    }

    // hết thời gian => tắt LED
    if (led_on && (int32_t)(now - led_off_time) >= 0)
    {
        led_on = 0;
        GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    }
}
