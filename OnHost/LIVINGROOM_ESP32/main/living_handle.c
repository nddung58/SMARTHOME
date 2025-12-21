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

#include "door.h"

#define DELTA_LUX 10.0f
#define DELTA_PPM 5.0f
#define DELTA_TEMP 0.5f
#define DELTA_HUMI 2.0f

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
    float delta = get_delta(id);
    return fabsf(new_value - old_value) > delta;
}

extern FrameQueue livQueue;

void living_handle_manual_task(void *arg)
{
    while (1)
    {
        uint8_t mode_his;
        storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), &mode_his);

        if ((!empty(&livQueue)) && (mode_his == (uint8_t)MANUAL_MODE))
        {
            message_t message;
            uint8_t data[FRAME_MAX_SIZE];

            if (front(&livQueue, data))
            {
                Message_Decode(data, &message);

                if (message.header[0] == COMMAND)
                {
                    switch (message.header[1])
                    {
                    case LED:
                        led_set((LED_State_t)message.payload[0]);
                        break;

                    case DOOR:
                        if (message.payload[0] == 0)
                            door_close();
                        else
                            door_open();
                        break;

                    case BUZZER:
                        if (message.payload[0] == 0)
                            buzzer_off();
                        else
                            buzzer_on();
                        break;

                    case MODE:
                        if (message.payload[0] == (uint8_t)AUTO_MODE)
                        {
                            storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), message.payload[0]);
                            char buffer[32];
                            snprintf(buffer, sizeof(buffer), "%d", message.payload[0]);
                            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, MODE), buffer, strlen(buffer));
                        }
                        break;
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
    while (1)
    {
        uint8_t h, h_d, t, t_d;
        if (dht11_read_data(&h, &h_d, &t, &t_d))
        {
            float humi = h + h_d / 10.0f;
            float temp = t + t_d / 10.0f;
            float humi_his;
            float temp_his;

            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), &humi_his);
            if (sensor_changed(HUMI, humi_his, humi))
            {
                storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), humi);

                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.2f", humi);
                mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), buffer, strlen(buffer));
            }

            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), &temp_his);
            if (sensor_changed(TEMP, temp_his, temp))
            {
                storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), temp);

                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.2f", temp);
                mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), buffer, strlen(buffer));
            }
        }

        float lux = ldr_getlux();
        float lux_his;
        storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), &lux_his);
        if (sensor_changed(LDR, lux_his, lux))
        {
            storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), lux);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", lux);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, LDR), buffer, strlen(buffer));
        }

        float gas = mq2_getppm();
        float gas_his;
        storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), &gas_his);
        if (sensor_changed(MQ2, gas_his, gas))
        {
            storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), gas);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", gas);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), buffer, strlen(buffer));
        }

        uint8_t led = (uint8_t)led_getstatus();
        uint8_t led_his;
        storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), &led_his);
        if (led != led_his)
        {
            storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), led);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", led);
            mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, LED), buffer, strlen(buffer));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void living_handle_auto_task(void *arg)
{

    while (1)
    {
        uint8_t mode_his;
        storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), &mode_his);

        if (mode_his == (uint8_t)AUTO_MODE)
        {
            float temp, humi, gas, lux;
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), &temp);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), &humi);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), &gas);
            storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), &lux);

            if (temp > 35.0f || humi < 35.0f || gas > 500)
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

            while (!empty(&livQueue))
            {
                message_t message;
                uint8_t data[FRAME_MAX_SIZE];

                if (!front(&livQueue, data))
                    break;
                if (!Message_Decode(data, &message))
                    break;

                if (message.header[0] == COMMAND && message.header[1] == MODE)
                {
                    if (message.payload[0] == (uint8_t)MANUAL_MODE)
                    {
                        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), message.payload[0]);
                    }
                }

                pop(&livQueue);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void living_handle_init(void)
{

    dht11_init();
    ldr_init();
    mq2_init();
    led_init();
    buzzer_init();

    door_init();

    uint8_t mode;
    if (storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), &mode) != ESP_OK)
    {
        mode = MANUAL_MODE;
        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), mode);
    }

    uint8_t led;
    if (storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), &led) != ESP_OK)
    {
        led = LED_OFF;
        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, LED), led);
    }

    uint8_t buzzer;
    if (storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, BUZZER), &buzzer) != ESP_OK)
    {
        buzzer = 0;
        storage_nvs_set_uint8(get_key_topic(ON_HOST_LIVING_ROOM, BUZZER), buzzer);
    }

    float fval;
    if (storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), &fval) != ESP_OK)
    {
        fval = 0.0f;
        storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, TEMP), fval);
    }

    if (storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), &fval) != ESP_OK)
    {
        fval = 0.0f;
        storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, HUMI), fval);
    }

    if (storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), &fval) != ESP_OK)
    {
        fval = 0.0f;
        storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, LDR), fval);
    }

    if (storage_nvs_get_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), &fval) != ESP_OK)
    {
        fval = 0.0f;
        storage_nvs_set_float(get_key_topic(ON_HOST_LIVING_ROOM, MQ2), fval);
    }

    xTaskCreate(
        living_handle_manual_task,
        "living_manual_task",
        4096,
        NULL,
        5,
        NULL);

    xTaskCreate(
        living_sensor_update_task,
        "living_sensor_update_task",
        4096,
        NULL,
        6,
        NULL);

    xTaskCreate(
        living_handle_auto_task,
        "living_auto_task",
        4096,
        NULL,
        4,
        NULL);
}