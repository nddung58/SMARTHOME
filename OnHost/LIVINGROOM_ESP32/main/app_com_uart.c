#include "app_com_uart.h"

#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "message.h"
#include "queue.h"

#include "esp_timer.h"

#include "ndd_std_types.h"

#include "storage_nvs.h"

#include "nddmqtt_client.h"
#include "ndd_std_types.h"
#include "string.h"

static const char *TAG = "APP_UART";

#define UART_NUM UART_NUM_1
#define UART_RX 16
#define UART_TX 17
#define BUF_SIZE 1024

static QueueHandle_t uart_queue;

extern FrameQueue txQueue;

static message_t message;
uint8_t uart_buffer[FRAME_MAX_SIZE];

void COM_UART_Init(uint32_t baud_rate)
{
    // Tạo hàng đợi UART event (nếu bạn dùng event, nhưng đang NULL ở dưới → chưa cần thiết nếu chỉ dùng polling/interrupt)
    uart_queue = xQueueCreate(20, sizeof(uart_event_t));
    if (uart_queue == NULL)
    {
        ESP_LOGI(TAG, "Failed to create UART queue\n");
        return;
    }

    // Cấu hình UART
    uart_config_t uart_config = {
        .baud_rate = (int)baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 20, &uart_queue, 0);

    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 12, NULL); // Priority 12 cho task xử lý sự kiện UART

    xTaskCreate(uart_tx_task, "uart_tx_task", 2048, NULL, 10, NULL); // Priority 10 cho task gửi dữ liệu qua UART

    ESP_LOGI(TAG, "UART config sucessfuly!");
}

void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t data[BUF_SIZE];

    while (1)
    {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY))
        {
            if (event.type == UART_DATA)
            {

                int len = uart_read_bytes(UART_NUM, data, event.size, portMAX_DELAY);
                for (int i = 0; i < len; i++)
                {
                    Fsm_Get_Message(data[i], uart_buffer);

                    if (Check_Fsm_Flag_New_Message())
                    {

                        if (Message_Decode(uart_buffer, &message))
                        {
                            if (message.header[0] == NOTIFY)
                            {
                                COM_HandleNotifyMessage();
                            }
                            else if (message.header[0] == RESPONSE)
                            {
                                COM_HandleResponseMessage(&txQueue);
                            }
                        }

                        Clear_All_State_Fsm();
                    }
                }
            }
        }
    }
}
typedef enum
{
    WAITING_TO_SEND,
    WAITING_FOR_RESPONSE,
    RESPONSE_ACKED,
    RESPONSE_NACKED
} uart_tx_state_t;

volatile uart_tx_state_t tx_state = WAITING_TO_SEND;
static uint32_t retry_count = 0;
static uint32_t last_sent_time = 0;

void uart_tx_task(void *param)
{
    while (1)
    {
        switch (tx_state)
        {
        case WAITING_TO_SEND:
            if (!empty(&txQueue))
            {
                uint8_t data[FRAME_MAX_SIZE];
                front(&txQueue, data);

                uart_write_bytes(UART_NUM, data, FRAME_MAX_SIZE);
                tx_state = WAITING_FOR_RESPONSE;
                last_sent_time = esp_timer_get_time() / 1000;
            }
            break;

        case WAITING_FOR_RESPONSE:
            // Timeout sau 1000ms nếu không nhận được phản hồi
            if ((esp_timer_get_time() / 1000 - last_sent_time) > 50)
            {
                ESP_LOGI(TAG, "Timeout khi chờ phản hồi!\n");
                tx_state = RESPONSE_NACKED;
            }
            break;

        case RESPONSE_ACKED:
            ESP_LOGI(TAG, "ACK nhận thành công. Gửi frame kế tiếp.\n");
            pop(&txQueue);
            retry_count = 0; // Reset khi đã gửi thành công
            tx_state = WAITING_TO_SEND;
            break;

        case RESPONSE_NACKED:
            retry_count++;
            if (retry_count <= 5)
            {
                ESP_LOGI(TAG, "NACK hoặc timeout, thử lại lần %ld\n", retry_count);
                tx_state = WAITING_TO_SEND;
            }
            else
            {
                ESP_LOGI(TAG, "Thất bại sau 5 lần. Bỏ qua frame.");
                pop(&txQueue);
                retry_count = 0;
                tx_state = WAITING_TO_SEND;
            }
            break;

        default:
            tx_state = WAITING_TO_SEND;
            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void COM_HandleNotifyMessage(void)
{

    switch (message.header[1])
    {
    case LDR:
    {
        if (message.header[2] == 4)
        {
            float value = Convert_Bytes_To_Float(message.payload[0], message.payload[1], message.payload[2], message.payload[3]);
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, LDR), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, LDR), buffer, strlen(buffer));
        }
        break;
    }
    case MQ2:
    {
        if (message.header[2] == 4)
        {
            float value = Convert_Bytes_To_Float(message.payload[0], message.payload[1], message.payload[2], message.payload[3]);
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, MQ2), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, MQ2), buffer, strlen(buffer));
        }

        break;
    }
    case TEMP:
    {
        if (message.header[2] == 4)
        {
            float value = Convert_Bytes_To_Float(message.payload[0], message.payload[1], message.payload[2], message.payload[3]);
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, TEMP), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, TEMP), buffer, strlen(buffer));
        }

        break;
    }
    case HUMI:
    {
        if (message.header[2] == 4)
        {
            float value = Convert_Bytes_To_Float(message.payload[0], message.payload[1], message.payload[2], message.payload[3]);
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, HUMI), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, HUMI), buffer, strlen(buffer));
        }

        break;
    }
    case RAIN:
    {
        if (message.header[2] == 1)
        {
            uint8_t value = message.payload[0];
            // storage_nvs_set_uint8(get_key_topic(ON_BOARD_BED_ROOM, RAIN), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, RAIN), buffer, strlen(buffer));
        }
        break;
    }
    case LED:
    {
        if (message.header[2] == 1)
        {
            uint8_t value = message.payload[0];
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, LED), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, LED), buffer, strlen(buffer));
        }
        break;
    }
    case FAN:
    {
        if (message.header[2] == 1)
        {
            uint8_t value = message.payload[0];
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, FAN), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, FAN), buffer, strlen(buffer));
        }
    }
    break;
    case BUZZER:
    {
        if (message.header[2] == 1)
        {
            uint8_t value = message.payload[0];
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, BUZZER), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, BUZZER), buffer, strlen(buffer));
        }
        break;
    }
    case AWNINGS:
    {
        if (message.header[2] == 1)
        {
            uint8_t value = message.payload[0];
            // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, AWNINGS), value);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", value);
            mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, AWNINGS), buffer, strlen(buffer));
        }
        break;
    }
    case MODE:
    {
        uint8_t value = message.payload[0];
        // storage_nvs_set_float(get_key_topic(ON_BOARD_BED_ROOM, MODE), value);

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", value);
        mqtt_pub(get_key_topic(ON_BOARD_BED_ROOM, MODE), buffer, strlen(buffer));
        break;
    }
    }
}

void COM_HandleResponseMessage(FrameQueue *queue)
{
    if (!empty(queue))
    {
        if (message.payload[0] == RESPONSE_ACK)
        {
            tx_state = RESPONSE_ACKED;
        }
        else if (message.payload[0] == RESPONSE_NACK)
        {
            tx_state = RESPONSE_NACKED;
        }
    }
}
