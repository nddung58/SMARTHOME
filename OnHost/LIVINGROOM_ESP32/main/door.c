#include "door.h"

#include "sg90.h"
#include "rfid.h"

#include "stdint.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nddmqtt_client.h"
#include "ndd_std_types.h"

static bool door_is_open = false;

static const char *TAG = "DOOR";

static void door_handle(uint8_t event)
{
    switch (event)
    {
    case ACCESS_GRANTED:
        ESP_LOGI(TAG, "Door: OPEN");
        if (!door_open())
        {
            printf("open fail!");
        }
        door_is_open = true;
        break;

    case CARD_REMOVED:
        if (door_is_open)
        {
            ESP_LOGI(TAG, "Door: CLOSE");
            vTaskDelay(pdMS_TO_TICKS(1000));
            if (!door_close())
            {
                printf("close fail!");
            }
            door_is_open = false;
        }
        break;

    case ACCESS_DENIED:
        break;
    }
}

void door_init(void)
{
    sg90_init();
    rfid_init();
    rfid_set_callback(door_handle);
}

bool door_open(void)
{
    for (int i = 0; i <= 90; i++)
    {
        if (!sg90_set_angle(i))
        {
            return false;
        }
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", 1);
    mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, DOOR), buffer, strlen(buffer));
    return true;
}

bool door_close(void)
{
    for (int i = 90; i >= 0; i--)
    {
        if (!sg90_set_angle(i))
        {
            return false;
        }
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", 0);
    mqtt_pub(get_key_topic(ON_HOST_LIVING_ROOM, DOOR), buffer, strlen(buffer));
    return true;
}