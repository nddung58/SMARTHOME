#include "pir.h"
#include "stm32_gpio.h"
#include "timer_base.h"
#include "stdint.h"
#include "door.h"

#define PIR_GPIO_PORT GPIOA
#define PIR_GPIO_PIN  GPIO_PIN_5


#define PIR_LOCK_MS      2000
#define PIR_LED_ON_MS    2000

volatile uint8_t pir_trigger = 0;
volatile uint32_t pir_last_trigger = 0;

static uint8_t led_on = 0;
static uint32_t led_off_time = 0;


void pir_init(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin  = PIR_GPIO_PIN,
        .Mode = GPIO_MODE_IT_RISING,
        .Pull = GPIO_PULLDOWN,
    };
    GPIO_Init(PIR_GPIO_PORT, &gpio);

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 2);
}


volatile uint8_t servo_trigger_flag = 0;

void GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PIR_GPIO_PIN)
    {
    	servo_trigger_flag = 1;
    }
}

void EXTI9_5_IRQHandler(void)
{
    GPIO_EXTI_IRQHandler(PIR_GPIO_PIN);
}


typedef enum {
    SERVO_IDLE,
    SERVO_WAIT_TO_CLOSE
} ServoState_t;

void process_servo_logic(void)
{
    static ServoState_t state = SERVO_IDLE;
    static uint32_t start_time = 0;

    switch (state)
    {
        case SERVO_IDLE:
            if (servo_trigger_flag)
            {
                servo_trigger_flag = 0;
                door_close();
                start_time = GetTick();
                state = SERVO_WAIT_TO_CLOSE;
            }
            break;

        case SERVO_WAIT_TO_CLOSE:

            if (GetTick() - start_time >= 2000)
            {
            	door_open();
                state = SERVO_IDLE;
            }
            break;
    }
}

