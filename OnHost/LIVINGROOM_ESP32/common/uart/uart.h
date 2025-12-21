#ifndef UART_H
#define UART_H

#include <stdint.h>

#define EX_UART_NUM UART_NUM_0
#define PATTERN_BYTE_NUM (3) 

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

typedef void (*uart_handler_t)(uint8_t *data, uint16_t length);
void uart_init(void);
void uart_set_callback(void *cb);
void uart_put(uint8_t *data, uint16_t length);

#endif // UART_H