#ifndef APP_DHT11_H_
#define APP_DHT11_H_

#include <stdint.h>
#include <stdbool.h>

void dht11_init(void);

bool dht11_read_data(uint8_t *humi_int, uint8_t *humi_dec, uint8_t *temp_int, uint8_t *temp_dec);

#endif // APP_DHT11_H_