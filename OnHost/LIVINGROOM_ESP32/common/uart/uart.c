#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"  
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "uart.h"

static const char *TAG = "UART";

static QueueHandle_t uart_queue;
static uart_handler_t p_uart_handler = NULL;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *) malloc(RD_BUF_SIZE);
    for(;;) {
        // Waiting for UART event
        if (xQueueReceive(uart_queue, (void * )&event, portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            switch (event.type) {
                case UART_DATA: {
                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    p_uart_handler(dtmp, event.size);
                    break;
                }
                case UART_FIFO_OVF:
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_BREAK:
                    ESP_LOGI(TAG, "UART[%d] break detected", EX_UART_NUM);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "UART[%d] parity error", EX_UART_NUM);
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "UART[%d] frame error", EX_UART_NUM);
                    break;
                default:
                    ESP_LOGI(TAG, "UART[%d] unknown event type: %d", EX_UART_NUM, event.type);
                    break;
            }
        }
    }
}

void uart_init(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    // Install UART driver
    uart_driver_install(EX_UART_NUM, RD_BUF_SIZE * 2, BUF_SIZE*2, 20, &uart_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    
   esp_log_level_set(TAG, ESP_LOG_INFO);

   uart_set_pin(EX_UART_NUM, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Create a task to handle UART events
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}   

void uart_set_callback(void *cb)
{
    if(cb){
        p_uart_handler = cb;
    } 
}

void uart_put(uint8_t *data, uint16_t length)
{
    // Write data to UART
    uart_write_bytes(EX_UART_NUM, (const char *)data, length);
}