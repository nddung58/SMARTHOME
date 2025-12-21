/*
 * led_rgb.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef INC_LED_RGB_H_
#define INC_LED_RGB_H_

#include <stdint.h>

typedef enum
{
    LED_OFF = 0,
    LED_WHITE_50 = 1,  // trắng 50% (mờ)
    LED_WHITE_100 = 2, // trắng 100% (sáng)
    LED_GREEN = 3,     // xanh lá cây
    LED_RED = 4,       // đỏ
    LED_UNKNOWN = 5
} LED_State_t;

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB_Color_t;

/**
 * @brief Khởi tạo LED RGB.
 *
 * Cấu hình các chân GPIO và Timer (PWM) tương ứng với 3 kênh R, G, B.
 * Hàm này cần được gọi một lần trước khi sử dụng LED_RGB_Set().
 */
void LED_RGB_Init(void);

/**
 * @brief Thiết lập độ sáng của từng màu R, G, B theo phần trăm.
 *
 * @param r_percent Độ sáng kênh đỏ (0–100).
 * @param g_percent Độ sáng kênh xanh lá (0–100).
 * @param b_percent Độ sáng kênh xanh dương (0–100).
 */
void LED_RGB_Set(uint8_t r_percent, uint8_t g_percent, uint8_t b_percent);

/**
 * @brief Tắt toàn bộ LED RGB (cả 3 kênh R, G, B).
 */
void LED_RGB_Off(void);

/**
 * @brief Đọc lại độ sáng hiện tại của từng kênh R, G, B.
 *
 * @param r_percent Con trỏ nhận độ sáng của kênh đỏ (0–255).
 * @param g_percent Con trỏ nhận độ sáng của kênh xanh lá (0–255).
 * @param b_percent Con trỏ nhận độ sáng của kênh xanh dương (0–255).
 */
void LED_RGB_Get(uint8_t *r_percent, uint8_t *g_percent, uint8_t *b_percent);

/**
 * @brief Thiết lập state LED RGB theo trạng thái đã định nghĩa.
 *
 * @param state Trạng thái LED cần thiết lập.
 *              - LED_OFF: Tắt LED
 *             - LED_WHITE_50: Trắng 50%
 *             - LED_WHITE_100: Trắng 100%
 *           - LED_WARM_WHITE: Vàng ngà
 *            - LED_GREEN: Xanh lá
 *           - LED_RED: Đỏ
 */
void LED_RGB_SetState(LED_State_t state);

#endif /* INC_LED_RGB_H_ */
