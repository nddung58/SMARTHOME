#ifndef STORAGE_NVS_H
#define STORAGE_NVS_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t storage_nvs_get_uint8(const char *key, uint8_t *value);
esp_err_t storage_nvs_set_uint8(const char *key, uint8_t value);

esp_err_t storage_nvs_get_int(const char *key, int *value);
esp_err_t storage_nvs_set_int(const char *key, int value);

esp_err_t storage_nvs_get_float(const char *key, float *value);
esp_err_t storage_nvs_set_float(const char *key, float value);

esp_err_t storage_nvs_get_string(const char *key, char *out);
esp_err_t storage_nvs_set_string(const char *key, char *str);

#endif