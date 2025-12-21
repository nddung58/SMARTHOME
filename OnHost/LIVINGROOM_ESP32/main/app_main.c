#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "storage_nvs.h"

#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "esp_system.h"

#include "input.h"
#include "wifi_config.h"

#include "ndd_std_types.h"

#include "nddmqtt_client.h"

#include "queue.h"
#include "app_com_uart.h"

#include "living_handle.h"

static const char *TAG = "APP_MAIN";

FrameQueue txQueue;

FrameQueue livQueue;

static const char *topic_cmd_table[NODE_MAX][DEVICE_MAX] = {
    [ON_HOST_LIVING_ROOM] = {
        [LDR] = TOPIC_CMD("living", "ldr"),
        [PIR] = TOPIC_CMD("living", "pir"),
        [MQ2] = TOPIC_CMD("living", "mq2"),
        [HUMI] = TOPIC_CMD("living", "humi"),
        [TEMP] = TOPIC_CMD("living", "temp"),
        [RAIN] = TOPIC_CMD("living", "rain"),
        [LED] = TOPIC_CMD("living", "led"),
        [FAN] = TOPIC_CMD("living", "fan"),
        [BUZZER] = TOPIC_CMD("living", "buzzer"),
        [AWNINGS] = TOPIC_CMD("living", "awnings"),
        [DOOR] = TOPIC_CMD("living", "door"),
        [MODE] = TOPIC_CMD("living", "mode"),
    },

    [ON_BOARD_BED_ROOM] = {
        [LDR] = TOPIC_CMD("bed", "ldr"),
        [PIR] = TOPIC_CMD("bed", "pir"),
        [MQ2] = TOPIC_CMD("bed", "mq2"),
        [HUMI] = TOPIC_CMD("bed", "humi"),
        [TEMP] = TOPIC_CMD("bed", "temp"),
        [RAIN] = TOPIC_CMD("bed", "rain"),
        [LED] = TOPIC_CMD("bed", "led"),
        [FAN] = TOPIC_CMD("bed", "fan"),
        [BUZZER] = TOPIC_CMD("bed", "buzzer"),
        [AWNINGS] = TOPIC_CMD("bed", "awnings"),
        [DOOR] = TOPIC_CMD("bed", "door"),
        [MODE] = TOPIC_CMD("bed", "mode"),
    }};

int build_topic_sub_list(const char **out_list)
{
    int index = 0;

    for (int host = 0; host < NODE_MAX; host++)
    {
        for (int dev = 0; dev < DEVICE_MAX; dev++)
        {
            const char *topic = topic_cmd_table[host][dev];
            if (topic != NULL)
            {
                out_list[index++] = topic;
            }
        }
    }
    return index;
}

#define MAX_TOPICS (NODE_MAX * DEVICE_MAX)

static const char *topics[MAX_TOPICS];

// Reset ESP32
typedef struct
{
    int gpio_num;
    uint32_t press_time_ms;
} button_event_t;

static QueueHandle_t button_evt_queue;

void button_task(void *arg)
{
    button_event_t evt;
    while (1)
    {
        if (xQueueReceive(button_evt_queue, &evt, portMAX_DELAY))
        {
            if (evt.press_time_ms > 3000)
            {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ESP_LOGI(TAG, "NVS erased. Restarting...");

                vTaskDelay(pdMS_TO_TICKS(500));

                esp_restart();
            }
        }
    }
}

void input_button_callback(int gpio_num, uint64_t tick)
{
    if (gpio_num == GPIO_NUM_0)
    {
        button_event_t evt = {.gpio_num = gpio_num,
                              .press_time_ms = tick * portTICK_PERIOD_MS};
        xQueueSendFromISR(button_evt_queue, &evt, NULL);
    }
}

void cmd_handle_liv(id_end_device_t id, char *data)
{
    if (data == NULL)
    {
        return;
    }

    uint8_t mode_his;
    storage_nvs_get_uint8(get_key_topic(ON_HOST_LIVING_ROOM, MODE), &mode_his);

    if ((mode_his != (uint8_t)MANUAL_MODE) && (id != MODE))
    {
        ESP_LOGI(TAG, "handle fail, auto mode!");
        return;
    }
    switch (id)
    {
    case LED:
        uint8_t led = (uint8_t)atoi(data);
        message_t msg_led;
        Create_Message(COMMAND, id, 1, &led, &msg_led);
        if (push(&livQueue, &msg_led) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case FAN:
        uint8_t fan = (uint8_t)atoi(data);
        message_t msg_fan;
        Create_Message(COMMAND, id, 1, &fan, &msg_fan);
        if (push(&livQueue, &msg_fan) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case BUZZER:
        uint8_t buz = (uint8_t)atoi(data);
        message_t msg_buz;
        Create_Message(COMMAND, id, 1, &buz, &msg_buz);
        if (push(&livQueue, &msg_buz) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case AWNINGS:
        uint8_t awn = (uint8_t)atoi(data);
        message_t msg_awn;
        Create_Message(COMMAND, id, 1, &awn, &msg_awn);
        if (push(&livQueue, &msg_awn) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case DOOR:
        uint8_t door = (uint8_t)atoi(data);
        message_t msg_door;
        Create_Message(COMMAND, id, 1, &door, &msg_door);
        if (push(&livQueue, &msg_fan) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case MODE:
        uint8_t m = (uint8_t)atoi(data);
        message_t msg_mode;
        Create_Message(COMMAND, id, 1, &m, &msg_mode);
        if (push(&livQueue, &msg_mode) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
    default:

        break;
    }
}

void cmd_handle_bed(id_end_device_t id, char *data)
{
    if (data == NULL)
    {
        return;
    }

    uint8_t mode_his;
    storage_nvs_get_uint8(get_key_topic(ON_BOARD_BED_ROOM, MODE), &mode_his);

    if ((mode_his != (uint8_t)MANUAL_MODE) && (id != MODE))
    {
        ESP_LOGI(TAG, "handle fail, auto mode!");
        return;
    }
    switch (id)
    {
    case LED:
        uint8_t led = (uint8_t)atoi(data);
        message_t msg_led;
        Create_Message(COMMAND, id, 1, &led, &msg_led);
        if (push(&txQueue, &msg_led) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case FAN:
        uint8_t fan = (uint8_t)atoi(data);
        message_t msg_fan;
        Create_Message(COMMAND, id, 1, &fan, &msg_fan);
        if (push(&txQueue, &msg_fan) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case BUZZER:
        uint8_t buz = (uint8_t)atoi(data);
        message_t msg_buz;
        Create_Message(COMMAND, id, 1, &buz, &msg_buz);
        if (push(&txQueue, &msg_buz) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case AWNINGS:
        uint8_t awn = (uint8_t)atoi(data);
        message_t msg_awn;
        Create_Message(COMMAND, id, 1, &awn, &msg_awn);
        if (push(&txQueue, &msg_awn) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case DOOR:
        uint8_t door = (uint8_t)atoi(data);
        message_t msg_door;
        Create_Message(COMMAND, id, 1, &door, &msg_door);
        if (push(&txQueue, &msg_fan) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    case MODE:
        uint8_t m = (uint8_t)atoi(data);
        message_t msg_mode;
        Create_Message(COMMAND, id, 1, &m, &msg_mode);
        if (push(&txQueue, &msg_mode) == false)
        {
            ESP_LOGI(TAG, "PUSH FAIL!");
        }
        break;
    default:
        break;
    }
}

void mqtt_handle(char *topic, int topic_len, char *data, int len)
{
    topic_info_t info = parse_topic(topic);

    if (info.type == TOPIC_INVALID)
        return;

    if (info.type == TOPIC_CMD)
    {
        if (info.node == ON_HOST_LIVING_ROOM)
        {
            cmd_handle_liv(info.dev, data);
        }
        else if (info.node == ON_BOARD_BED_ROOM)
        {
            cmd_handle_bed(info.dev, data);
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    Queue_init(&txQueue);
    Queue_init(&livQueue);
    living_handle_init();

    wifi_config();
    int count = build_topic_sub_list(topics);
    mqtt_init(topics, count);
    mqtt_set_callback(mqtt_handle);
    mqtt_start();

    vTaskDelay(pdMS_TO_TICKS(500));

    COM_UART_Init(115200);

    input_set_callback(input_button_callback);
    input_io_create(GPIO_NUM_0, ANY_EDGE);

    button_evt_queue = xQueueCreate(4, sizeof(button_event_t));
    xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);
}
