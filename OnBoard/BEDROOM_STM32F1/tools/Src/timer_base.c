#include "timer_base.h"

static volatile uint32_t tick_ms = 0; // Bộ đếm ms

static TIM_HandleTypeDef htim2;

void Timer_Init(void)
{
	 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Cấu hình TIM2 cho 1ms (SystemCoreClock = 72MHz)
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 72 - 1;     // 1 MHz (1 tick = 1us)
    htim2.Init.Period = 1000-1;      // 1000 ticks = 1ms
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    TIM_Base_Init(&htim2);

    // Bật update interrupt
    htim2.Instance->DIER |= TIM_DIER_UIE;

    // Bật timer
    TIM_ENABLE(&htim2);



    // Bật ngắt TIM2 trong NVIC
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void IncTick(void)
{
    tick_ms++;
}

uint32_t GetTick(void)
{
    return tick_ms;
}

void Delay_ms(uint32_t ms)
{
    uint32_t start = GetTick();
    while ((GetTick() - start) < ms);
}

void Delay_us(uint32_t us)
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
	        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
	        DWT->CYCCNT = 0;
	        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	    }

	    uint32_t start = DWT->CYCCNT;
	    uint32_t ticks = us * (SystemCoreClock / 1000000);
	    while ((DWT->CYCCNT - start) < ticks);
}

// Handler xử lý ngắt TIM2
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) // Kiểm tra cờ ngắt Update Interrupt Flag
    {
        TIM2->SR &= ~TIM_SR_UIF;
        IncTick();
    }
}
