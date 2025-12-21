/*
 * Utility functions for URI encoding/decoding
 *
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Encode a URI
 *
 * @param dest  Destination buffer
 * @param src   Source string
 * @param len   Length of the source string
 * @return uint32_t  Number of encoded characters
 *
 * @note Allocate dest buffer with size up to 3x of src length.
 */
uint32_t uri_encode(char *dest, const char *src, size_t len);

/**
 * @brief Decode a URI
 *
 * @param dest  Destination buffer
 * @param src   Source string
 * @param len   Length of the source string
 *
 * @note Allocate dest buffer same size as src at minimum.
 */
void uri_decode(char *dest, const char *src, size_t len);

#ifdef __cplusplus
}
#endif
