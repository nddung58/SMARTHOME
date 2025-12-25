#include "living_handle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "math.h"
#include "message.h"
#include "queue.h"
#include "ndd_std_types.h"
#include "storage_nvs.h"
#include "nddmqtt_client.h"
#include "ldr.h"
#include "mq2.h"
#include "dht11.h"
#include "led.h"
#include "buzzer.h"
#include "esp_log.h"
#include "door.h"
#include "rfid.h"

static const char *TAG = "LIVING";

#define DELTA_LUX 10.0f
#define DELTA_PPM 5.0f
#define DELTA_TEMP 0.5f
#define DELTA_HUMI 2.0f

static uint8_t g_app_mode = AUTO_MODE;

static float get_delta(id_end_device_t id)
{
    switch (id)
    {
    case LDR:
        return DELTA_LUX;
    case MQ2:
        return DELTA_PPM;
    case TEMP:
        return DELTA_TEMP;
    case HUMI:
        return DELTA_HUMI;
    default:
        return 0;
    }
}

bool sensor_changed(id_end_device_t id, float old_value, float new_value)
{
    return fabsf(new_value - old_value) > get_delta(id);
}

extern FrameQueue livQueue;

void living_handle_manual_task(void *arg)
{
    while (1)
    {
        if (!empty(&livQueue))
        {
            message_t message;
            uint8_t data[FRAME_MAX_SIZE];

            if (front(&livQueue, data))
            {
                Message_Decode(data, &message);

                if (message.header[0] == COMMAND)
                {
                    id_end_device_t device = (id_end_device_t)message.header[1];
                    uint8_t payload = message.payload[0];

                    if (device == MODE)
                    {
                        g_app_mode = payload;
                        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), g_app_mode);

                        char buf[8];
                        snprintf(buf, sizeof(buf), "%d", g_app_mode);
                        mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, MODE), buf, strlen(buf));

                        ESP_LOGI(TAG, "Mode changed to: %s", g_app_mode == AUTO_MODE ? "AUTO" : "MANUAL");
                    }
                    else if (g_app_mode == MANUAL_MODE)
                    {
                        switch (device)
                        {
                        case LED:
                            led_set((LED_State_t)payload);
                            break;
                        case DOOR:
                            if (payload == 0)
                                door_close();
                            else
                                door_open();
                            break;
                        case BUZZER:
                            if (payload == 0)
                                buzzer_off();
                            else
                                buzzer_on();
                            break;
                        default:
                            break;
                        }
                    }
                }
                pop(&livQueue);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void living_sensor_update_task(void *arg)
{
    dht11_t dht11_sensor;
    char buffer[32];

    while (1)
    {
        if (!dht11_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT))
        {
            float humi_his, temp_his;
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), &humi_his);
            if (sensor_changed(HUMI, humi_his, dht11_sensor.humidity))
            {
                storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), dht11_sensor.humidity);
                snprintf(buffer, sizeof(buffer), "%.2f", dht11_sensor.humidity);
                mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), buffer, strlen(buffer));
            }

            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), &temp_his);
            if (sensor_changed(TEMP, temp_his, dht11_sensor.temperature))
            {
                storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), dht11_sensor.temperature);
                snprintf(buffer, sizeof(buffer), "%.2f", dht11_sensor.temperature);
                mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), buffer, strlen(buffer));
            }
        }

        float lux = ldr_getlux();
        float lux_his;
        storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), &lux_his);
        if (sensor_changed(LDR, lux_his, lux))
        {
            storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), lux);
            snprintf(buffer, sizeof(buffer), "%.2f", lux);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, LDR), buffer, strlen(buffer));
        }

        float gas = mq2_getppm();
        float gas_his;
        storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), &gas_his);
        if (sensor_changed(MQ2, gas_his, gas))
        {
            storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), gas);
            snprintf(buffer, sizeof(buffer), "%.2f", gas);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), buffer, strlen(buffer));
        }

        uint8_t led = (uint8_t)led_getstatus();
        uint8_t led_his;
        storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), &led_his);
        if (led != led_his)
        {
            storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), led);
            snprintf(buffer, sizeof(buffer), "%d", led);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, LED), buffer, strlen(buffer));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void living_handle_auto_task(void *arg)
{
    while (1)
    {
        if (g_app_mode == AUTO_MODE)
        {
            float temp, humi, gas, lux;
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), &temp);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), &humi);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), &gas);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), &lux);

            if (temp > 35.0f || humi < 35.0f || gas > 500.0f)
            {
                buzzer_on();
                led_set(LED_RED);
                door_open();
            }
            else
            {
                buzzer_off();
                if (lux < 300.0f)
                    led_set(LED_WHITE_100);
                else if (lux < 600.0f)
                    led_set(LED_WHITE_50);
                else
                    led_set(LED_OFF);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void living_handle_init(void)
{
    ldr_init();
    mq2_init();
    led_init();
    buzzer_init();
    door_init();

    if (storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), &g_app_mode) != ESP_OK)
    {
        g_app_mode = AUTO_MODE;
        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), g_app_mode);
    }

    xTaskCreate(living_handle_manual_task, "manual_task", 4096, NULL, 5, NULL);
    xTaskCreate(living_sensor_update_task, "update_task", 4096, NULL, 6, NULL);
    xTaskCreate(living_handle_auto_task, "auto_task", 4096, NULL, 4, NULL);
}