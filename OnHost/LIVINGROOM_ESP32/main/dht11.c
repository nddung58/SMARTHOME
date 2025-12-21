#include "dht11.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHT11_PIN GPIO_NUM_4

static inline void dht_set_output(void)
{
    gpio_reset_pin(DHT11_PIN);
    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
}

static inline void dht_set_input(void)
{
    gpio_reset_pin(DHT11_PIN);
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}

static inline uint8_t dht_read_pin(void)
{
    return gpio_get_level(DHT11_PIN);
}

void dht11_init(void)
{
    dht_set_output();
    gpio_set_level(DHT11_PIN, 1);
}

static void dht11_start(void)
{
    dht_set_output();
    gpio_set_level(DHT11_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(DHT11_PIN, 1);
    esp_rom_delay_us(30);
    dht_set_input();
}

static uint8_t dht_read_bit(void)
{
    uint32_t t = 0;
    while (dht_read_pin() == 0)
    {
        if (++t > 10000)
            return 0;
        esp_rom_delay_us(1);
    }
    esp_rom_delay_us(40);
    return dht_read_pin();
}

static uint8_t dht_read_byte(void)
{
    uint8_t b = 0;
    for (int i = 0; i < 8; i++)
    {
        b <<= 1;
        b |= dht_read_bit();

        uint32_t t = 0;
        while (dht_read_pin())
        {
            if (++t > 10000)
                break;
            esp_rom_delay_us(1);
        }
    }
    return b;
}

bool dht11_read_data(uint8_t *humi_int, uint8_t *humi_dec, uint8_t *temp_int, uint8_t *temp_dec)
{
    if (!humi_int || !humi_dec || !temp_int || !temp_dec)
        return false;

    uint8_t buf[5] = {0};
    dht11_start();

    uint32_t t = 0;
    while (dht_read_pin())
        if (++t > 10000)
            return false;
    while (!dht_read_pin())
        if (++t > 10000)
            return false;
    while (dht_read_pin())
        if (++t > 10000)
            return false;

    for (int i = 0; i < 5; i++)
        buf[i] = dht_read_byte();

    if ((buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
        return false;

    *humi_int = buf[0];
    *humi_dec = buf[1];
    *temp_int = buf[2];
    *temp_dec = buf[3];
    return true;
}
