#ifndef FSM_MESSAGE_H
#define FSM_MESSAGE_H

#include <stdio.h>
#include <message.h>

/// @brief Các trạng thái của máy trạng thái (FSM) dùng để phân tích frame UART
typedef enum
{
    FSM_STATE_START = 0,   ///< Trạng thái bắt đầu - chờ byte START_BYTE
    FSM_STATE_HEADER = 1,  ///< Trạng thái nhận header (group, id, length)
    FSM_STATE_PAYLOAD = 2, ///< Trạng thái nhận dữ liệu payload
    FSM_STATE_CHECKSUM = 3 ///< Trạng thái nhận và kiểm tra checksum
} fsm_state_t;

/**
 * @brief Nhận một byte dữ liệu và xử lý theo FSM.
 * @param datain Byte đầu vào nhận được từ UART.
 * @param message message_t dùng để lưu toàn bộ frame nếu nhận thành công.
 * @return Số byte sẽ nhận ở lần tiếp theo.
 */
void Fsm_Get_Message(uint8_t datain, uint8_t dataout[]);

/**
 * @brief Reset trạng thái FSM và các biến liên quan.
 *
 * Gọi hàm này sau khi đã xử lý xong một message hợp lệ, để FSM sẵn sàng nhận message tiếp theo.
 */
void Clear_All_State_Fsm(void);

/**
 * @brief Kiểm tra xem đã nhận được một frame hoàn chỉnh và hợp lệ chưa.
 * @return 1 nếu đã nhận đủ và đúng frame, 0 nếu chưa.
 */
uint8_t Check_Fsm_Flag_New_Message(void);

#endif // FSM_MESSAGE_H
