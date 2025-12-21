#ifndef APP_MAIN_H
#define APP_MAIN_H

#include <stdint.h>
#include "ndd_std_types.h"

/**
 * @brief Cấu trúc lưu trữ trạng thái hệ thống.
 *
 * Bao gồm các giá trị đo từ cảm biến (nhiệt độ, độ ẩm, khí gas, ánh sáng)
 * và trạng thái của các thiết bị điều khiển như đèn, động cơ, còi.
 */
typedef struct
{
    float temperature; ///< Nhiệt độ từ cảm biến DHT11
    float humidity;    ///< Độ ẩm từ cảm biến DHT11
    float gas_level;   ///< Mức khí gas từ MQ2
    float lux;         ///< Mức ánh sáng từ LDR
    uint8_t rainpercent;

    uint8_t led_state; ///< Trạng thái bật/tắt LED (1: bật, 0: tắt)

    uint8_t fan;

    uint8_t buzzer_on; ///< Trạng thái còi báo động (1: bật, 0: tắt)

    uint8_t awnings;

    uint8_t door;

    mode_t mode; ///< Chế độ hoạt động hiện tại (AUTO hoặc MANUAL)
} SystemState;

/**
 * @brief Khởi tạo các thành phần trong ứng dụng.
 *
 * Gồm cảm biến, thiết bị điều khiển, giao tiếp và trạng thái ban đầu.
 */
void App_Init(void);

/**
 * @brief Hàm vòng lặp chính xử lý logic của ứng dụng.
 *
 * Thực thi logic điều khiển tùy thuộc vào chế độ (AUTO hoặc MANUAL),
 * xử lý dữ liệu cảm biến, gửi phản hồi và cập nhật thiết bị điều khiển.
 */
void App_Loop(void);

#endif // APP_MAIN_H
