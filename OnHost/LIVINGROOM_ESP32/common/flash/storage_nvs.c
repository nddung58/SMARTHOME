#include "storage_nvs.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

#define USER_NAMESPACE "__storage_nvs"

static nvs_handle_t my_handle;

esp_err_t storage_nvs_set_uint8(const char *key, uint8_t value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_u8(my_handle, key, value);
    if (err == ESP_OK)
        err = nvs_commit(my_handle);

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_set_int(const char *key, int value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_i32(my_handle, key, value);
    if (err == ESP_OK)
        err = nvs_commit(my_handle);

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_set_float(const char *key, float value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    uint32_t raw;
    memcpy(&raw, &value, sizeof(raw));

    err = nvs_set_u32(my_handle, key, raw);
    if (err == ESP_OK)
        err = nvs_commit(my_handle);

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_set_string(const char *key, char *str)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_str(my_handle, key, str);
    if (err == ESP_OK)
        err = nvs_commit(my_handle);

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_get_uint8(const char *key, uint8_t *value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_get_u8(my_handle, key, value);
    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_get_int(const char *key, int *value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    int32_t tmp;
    err = nvs_get_i32(my_handle, key, &tmp);
    if (err == ESP_OK)
        *value = tmp;

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_get_float(const char *key, float *value)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    uint32_t raw;
    err = nvs_get_u32(my_handle, key, &raw);
    if (err == ESP_OK)
        memcpy(value, &raw, sizeof(float));

    nvs_close(my_handle);
    return err;
}

esp_err_t storage_nvs_get_string(const char *key, char *out)
{
    esp_err_t err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
        return err;

    size_t length = 0;
    err = nvs_get_str(my_handle, key, NULL, &length);
    if (err != ESP_OK)
    {
        nvs_close(my_handle);
        return err;
    }

    err = nvs_get_str(my_handle, key, out, &length);
    nvs_close(my_handle);
    return err;
}
