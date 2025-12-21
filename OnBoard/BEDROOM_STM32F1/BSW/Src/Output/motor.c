#include "../../../BSW/Inc/Output/motor.h"

#include "stm32_gpio.h"
#include "stm32_tim.h"

static TIM_HandleTypeDef htim1;

#define MOTOR_PWM_CHANNEL TIM_CHANNEL_1

#define MOTOR_PWM_PORT GPIOA
#define MOTOR_PWM_PIN (1U << 8) // PA8 - DIR pin

#define MOTOR_IN1_PORT GPIOB
#define MOTOR_IN1_PIN (1U << 15) // PB15 - IN1 pin
#define MOTOR_IN2_PORT GPIOB
#define MOTOR_IN2_PIN (1U << 14) // PB14 - IN2 pin

void Motor_Init(void)
{
    // 1. Clock enable
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_TIM1EN;

    // 2. Init PA8: TIM1_CH1 (AF_PP)
    GPIO_InitTypeDef gpio_pwm = {
        .Pin = (MOTOR_PWM_PIN),
        .Mode = GPIO_MODE_AF_PP,
        .Speed = GPIO_SPEED_HIGH};
    GPIO_Init(MOTOR_PWM_PORT, &gpio_pwm);

    // 3. Init PB15: IN1 pin (output push-pull)
    GPIO_InitTypeDef gpio_in1 = {
        .Pin = (MOTOR_IN1_PIN),
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_LOW};
    GPIO_Init(MOTOR_IN1_PORT, &gpio_in1);

    // 4. Init PB14: IN2 pin (output push-pull)
    GPIO_InitTypeDef gpio_in2 = {
        .Pin = (MOTOR_IN2_PIN),
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_LOW};
    GPIO_Init(MOTOR_IN2_PORT, &gpio_in2);

    // 4. Timer config
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 71; // 72MHz / (71+1) = 1MHz
    htim1.Init.Period = 200;   // 1MHz / (199+1) = 5kHz PWM
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    TIM_PWM_Init(&htim1);

    // 5. PWM channel config
    TIM_OC_InitTypeDef oc = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE};

    TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_1);
    TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void Motor_SetSpeed(int8_t speed_percent)
{

    if (speed_percent > 100)
        speed_percent = 100;
    if (speed_percent < -100)
        speed_percent = -100;

    if (speed_percent >= 0)
    {
        GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_SET);
        GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
    }
    else
    {
        GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
        GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_SET);
        speed_percent = -speed_percent;
    }

    uint32_t compare = ((htim1.Init.Period + 1) * speed_percent) / 100;

    TIM_SetCompare(&htim1, MOTOR_PWM_CHANNEL, compare);
}

void Motor_Stop(void)
{
    TIM_SetCompare(&htim1, MOTOR_PWM_CHANNEL, 0);
}

void Motor_SetLevel(uint8_t state)
{
    if (state == 0)
    {
        Motor_Stop();
        GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
        GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
    }
    else if (state == 1)
    {

        Motor_SetSpeed(50); // Low speed
    }
    else if (state == 2)
    {
        Motor_SetSpeed(75); // Medium speed
    }
    else if (state == 3)
    {
        Motor_SetSpeed(100); // High speed
    }
}

uint8_t Motor_GetLevel(void)
{
    uint32_t period = htim1.Init.Period + 1;
    if (period == 0)
        return 0;

    uint32_t ccr = htim1.Instance->CCR1;
    uint8_t speed_percent = (ccr * 100) / period;

    if (speed_percent == 0)
        return 0; // STOP
    else if (speed_percent <= 60)
        return 1; // LOW
    else if (speed_percent <= 85)
        return 2; // MEDIUM
    else
        return 3; // HIGH
}
