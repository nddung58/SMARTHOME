#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"   
#include "spi.h"

#ifndef SPI_MAX_XFER_SZ
#define SPI_MAX_XFER_SZ 4096
#endif

static const char *TAG = "SPI";

static spi_device_handle_t spi_handle = NULL;
static spi_handler_t p_spi_handler = NULL;
static TaskHandle_t s_spi_worker = NULL;
static SemaphoreHandle_t s_dev_mutex = NULL;
static volatile bool s_shutting_down = false;

typedef struct
{
    uint8_t *tx_alloc; // DMA buffer cho TX (>4B)
    uint8_t *rx_alloc; // DMA bounce buffer cho RX (>4B) nếu rx_dest không DMA-capable
    uint8_t *rx_dest;  // Buffer đích của caller
    uint16_t len;
    uint8_t used_rxdata; // true nếu dùng rx_data (<=4B)
} spi_meta_t;

static void spi_powerup_guard(void)
{
    gpio_config_t cfg_in = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<<SPI_MOSI_PIN) | (1ULL<<SPI_MISO_PIN) | (1ULL<<SPI_CLK_PIN),
        .pull_down_en = 0,
        .pull_up_en   = 0
    };
    gpio_config(&cfg_in);

    // CS ở mức HIGH (không chọn slave)
    gpio_config_t cfg_cs = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<SPI_CS_PIN),
        .pull_down_en = 0,
        .pull_up_en   = 0
    };
    gpio_config(&cfg_cs);
    gpio_set_level(SPI_CS_PIN, 1);

    // Chờ S32K lên nguồn ổn định (200 ms)
    vTaskDelay(pdMS_TO_TICKS(200));
}

static inline bool spi_is_inited(void) { return (spi_handle != NULL); }

static void spi_free_meta(spi_meta_t *m, spi_transaction_t *t)
{
    if (!m)
        return;
    // copy từ bounce buffer về đích nếu có
    if (m->rx_alloc && m->rx_dest && m->len)
    {
        memcpy(m->rx_dest, m->rx_alloc, m->len);
    }
    // với trường hợp <=4B, worker đã xử lý bên dưới (rx_data)
    if (m->tx_alloc)
        free(m->tx_alloc);
    if (m->rx_alloc)
        free(m->rx_alloc);
    free(m);
    if (t)
        free(t);
}

static void spi_worker_task(void *arg)
{
    (void)arg;
    for (;;)
    {
        spi_transaction_t *ret_t = NULL;
        // timeout ngắn để có thể thoát khi shutdown
        esp_err_t ret = spi_device_get_trans_result(spi_handle, &ret_t, pdMS_TO_TICKS(100));
        if (ret == ESP_ERR_TIMEOUT)
        {
            if (s_shutting_down)
                break;
            continue;
        }
        if (ret != ESP_OK)
        {
            if (s_shutting_down)
                break;
            ESP_LOGE(TAG, "get_trans_result: %s", esp_err_to_name(ret));
            continue;
        }

        if (ret_t)
        {
            spi_meta_t *meta = (spi_meta_t *)ret_t->user;
            if (meta)
            {
                // nếu dùng rx_data (<=4B) thì copy sang rx_dest tại đây
                if (meta->used_rxdata && meta->rx_dest && meta->len <= 4)
                {
                    memcpy(meta->rx_dest, ret_t->rx_data, meta->len);
                }
                spi_free_meta(meta, ret_t);
            }
            else
            {
                free(ret_t);
            }
        }

        if (p_spi_handler)
            p_spi_handler(ESP_OK);
    }
    vTaskDelete(NULL);
}

void spi_init(void)
{
    if (spi_is_inited())
        return;

    s_shutting_down = false;

    spi_powerup_guard();

    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SPI_MAX_XFER_SZ};

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // 20 MHz
        .mode = 0,                         // CPOL=0, CPHA=0
        .spics_io_num = SPI_CS_PIN,
        .queue_size = SPI_QUEUE_SIZE,
        .flags = 0,            // FULL-DUPLEX
        .cs_ena_pretrans = 16,  // giữ CS thấp trước khi clock (cứu byte đầu)
        .cs_ena_posttrans = 8, // giữ CS thêm chút sau khi xong
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST_USED, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST_USED, &devcfg, &spi_handle));

    s_dev_mutex = xSemaphoreCreateMutex();
    if (!s_dev_mutex)
    {
        spi_bus_remove_device(spi_handle);
        spi_handle = NULL;
        spi_bus_free(SPI_HOST_USED);
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }

    BaseType_t ok = xTaskCreate(spi_worker_task, "spi_worker", 3072, NULL, tskIDLE_PRIORITY + 2, &s_spi_worker);
    if (ok != pdPASS)
    {
        vSemaphoreDelete(s_dev_mutex);
        s_dev_mutex = NULL;
        spi_bus_remove_device(spi_handle);
        spi_handle = NULL;
        spi_bus_free(SPI_HOST_USED);
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
}

void spi_deinit(void)
{
    if (!spi_is_inited())
        return;

    s_shutting_down = true;

    if (s_spi_worker)
    {
        vTaskDelete(s_spi_worker);
        s_spi_worker = NULL;
    }

    while (1)
    {
        spi_transaction_t *ret_t = NULL;
        esp_err_t ret = spi_device_get_trans_result(spi_handle, &ret_t, 0);
        if (ret == ESP_OK)
        {
            spi_meta_t *meta = ret_t ? (spi_meta_t *)ret_t->user : NULL;
            if (meta)
            {
                if (meta->used_rxdata && meta->rx_dest && meta->len <= 4)
                {
                    memcpy(meta->rx_dest, ret_t->rx_data, meta->len);
                }
                spi_free_meta(meta, ret_t);
            }
            else if (ret_t)
            {
                free(ret_t);
            }
            continue;
        }
        break;
    }

    if (s_dev_mutex)
    {
        vSemaphoreDelete(s_dev_mutex);
        s_dev_mutex = NULL;
    }

    spi_bus_remove_device(spi_handle);
    spi_handle = NULL;
    spi_bus_free(SPI_HOST_USED);
}

void spi_set_callback(spi_handler_t cb)
{
    p_spi_handler = cb;
}

esp_err_t spi_transfer_async_duplex(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    if (!spi_is_inited() || s_shutting_down)
        return ESP_ERR_INVALID_STATE;
    if ((!tx && !rx) || len == 0)
        return ESP_ERR_INVALID_ARG;
    if (len > SPI_MAX_XFER_SZ)
        return ESP_ERR_INVALID_SIZE;

    spi_transaction_t *t = (spi_transaction_t *)heap_caps_calloc(1, sizeof(*t), MALLOC_CAP_DEFAULT);
    if (!t)
        return ESP_ERR_NO_MEM;

    spi_meta_t *meta = (spi_meta_t *)heap_caps_calloc(1, sizeof(*meta), MALLOC_CAP_DEFAULT);
    if (!meta)
    {
        free(t);
        return ESP_ERR_NO_MEM;
    }

    t->length = (size_t)len * 8;

    if (len <= 4)
    {
        if (tx)
        {
            t->flags |= SPI_TRANS_USE_TXDATA;
            memcpy(t->tx_data, tx, len);
        }
        if (rx)
        {
            t->flags |= SPI_TRANS_USE_RXDATA;
            meta->rx_dest = rx;
            meta->used_rxdata = 1;
            meta->len = len;
        }
    }
    else
    {
        // TX: cấp phát DMA nếu cần
        if (tx)
        {
            uint8_t *dma_tx = (uint8_t *)heap_caps_malloc(len, MALLOC_CAP_DMA);
            if (!dma_tx)
            {
                free(meta);
                free(t);
                return ESP_ERR_NO_MEM;
            }
            memcpy(dma_tx, tx, len);
            t->tx_buffer = dma_tx;
            meta->tx_alloc = dma_tx;
        }
        // RX: nếu rx không DMA-capable, dùng bounce buffer
        if (rx)
        {
            if (!esp_ptr_dma_capable(rx))
            {
                uint8_t *dma_rx = (uint8_t *)heap_caps_malloc(len, MALLOC_CAP_DMA);
                if (!dma_rx)
                {
                    if (meta->tx_alloc)
                        free(meta->tx_alloc);
                    free(meta);
                    free(t);
                    return ESP_ERR_NO_MEM;
                }
                t->rx_buffer = dma_rx;
                meta->rx_alloc = dma_rx;
                meta->rx_dest = rx;
                meta->len = len;
            }
            else
            {
                t->rx_buffer = rx;
            }
        }
    }

    t->user = meta;

    esp_err_t ret;
    if (s_dev_mutex)
        xSemaphoreTake(s_dev_mutex, portMAX_DELAY);
    ret = spi_device_queue_trans(spi_handle, t, portMAX_DELAY);
    if (s_dev_mutex)
        xSemaphoreGive(s_dev_mutex);

    if (ret != ESP_OK)
    {
        if (meta)
        {
            if (meta->tx_alloc)
                free(meta->tx_alloc);
            if (meta->rx_alloc)
                free(meta->rx_alloc);
            free(meta);
        }
        free(t);
    }

    return ret;
}
