/*
 * dht11.c
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#include "../../../BSW/Inc/Input/dht11.h"

#include "timer_base.h"

#define DHT11_PORT GPIOA
#define DHT11_PIN (1U << 2) // PA2

static void DHT_SetPinOutput(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = DHT11_PIN,
        .Mode = GPIO_MODE_OUTPUT_OD,
        .Speed = GPIO_SPEED_LOW,
    };
    GPIO_Init(DHT11_PORT, &gpio);
}

static void DHT_SetPinInput(void)
{
    GPIO_InitTypeDef gpio = {
        .Pin = DHT11_PIN,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_NOPULL};
    GPIO_Init(DHT11_PORT, &gpio);
}

static uint8_t DHT_ReadPin(void)
{
    return (DHT11_PORT->IDR & DHT11_PIN) ? 1 : 0;
}

void DHT11_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    DHT_SetPinOutput();
    GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
}

static void DHT11_Start(void)
{
    DHT_SetPinOutput();
    GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    Delay_ms(20); // Kéo thấp ≥ 18ms
    GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    Delay_us(30);
    DHT_SetPinInput();
}

static uint8_t DHT_ReadBit(void)
{
    while (DHT_ReadPin() == 0)
        ;                 // Chờ đầu 0
    Delay_us(40);         // Sau 30us kiểm tra
    return DHT_ReadPin(); // HIGH → 1, LOW → 0
}

static uint8_t DHT_ReadByte(void)
{
    uint8_t i, byte = 0;
    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte |= DHT_ReadBit();
        while (DHT_ReadPin())
            ; // Chờ kết thúc bit
    }
    return byte;
}

bool DHT11_Read(uint8_t *humi_int, uint8_t *humi_dec, uint8_t *temp_int, uint8_t *temp_dec)
{
	if (!humi_int || !humi_dec || !temp_int || !temp_dec)
	        return false;
    uint8_t data[5] = {0};

    DHT11_Start();

    // Chờ phản hồi từ DHT11
    uint32_t timeout = 0;
    while (DHT_ReadPin())
        if (++timeout > 10000)
            return false;
    timeout = 0;
    while (!DHT_ReadPin())
        if (++timeout > 10000)
            return false;
    timeout = 0;
    while (DHT_ReadPin())
        if (++timeout > 10000)
            return false;

    // Đọc 5 byte
    for (uint8_t i = 0; i < 5; i++)
        data[i] = DHT_ReadByte();

    // Kiểm tra checksum
    if ((data[0] + data[1] + data[2] + data[3]) != data[4])
        return false;

    *humi_int = data[0];
    *humi_dec = data[1];
    *temp_int = data[2];
    *temp_dec = data[3];
    return true;
}
