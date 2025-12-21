#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "driver/spi_master.h"

#define SPI_HOST_USED      SPI2_HOST      

#define SPI_CLK_PIN        18
#define SPI_MOSI_PIN       23
#define SPI_MISO_PIN       19   
#define SPI_CS_PIN         5

#define SPI_QUEUE_SIZE     8

#define SPI_MAX_XFER_SZ    4096

typedef void (*spi_handler_t)(esp_err_t status);

void      spi_init(void);
void      spi_deinit(void);
void      spi_set_callback(spi_handler_t cb);
esp_err_t spi_transfer_async_duplex(const uint8_t *tx, uint8_t *rx, uint16_t len);