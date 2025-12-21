#ifndef GLOBALS_H
#define GLOBALS_H

#include "queue.h"
#include "app_main.h"
#include "../../BSW/Inc/Com/uart.h"

/// @brief Cấu trúc xử lý UART (được sử dụng cho giao tiếp UART chính).
extern UART_HandleTypeDef huart1;

/// @brief Hàng đợi dùng để lưu trữ các khung dữ liệu nhận qua UART.
extern FrameQueue g_uartQueue;

/// @brief Bộ đệm tạm thời để lưu từng byte nhận từ UART (kích thước 1 khung).
extern uint8_t uart_rx_buffer[FRAME_MAX_SIZE];

/// @brief Biến trạng thái hệ thống chính (dùng cho FSM hoặc logic điều khiển).
extern SystemState sys;

#endif // GLOBALS_H
