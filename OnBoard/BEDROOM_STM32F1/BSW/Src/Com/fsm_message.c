#include "../../../BSW/Inc/Com/fsm_message.h"

#include <string.h>
#include "message.h"

#include "../../../BSW/Inc/Com/uart.h"

static fsm_state_t fsm_state = FSM_STATE_START; // Trạng thái hiện tại của FSM
static uint8_t byte_count = 0;                  // Số byte đã nhận
static uint8_t expected_payload_length = 0;     // Độ dài payload lấy từ header
static uint8_t flag_fsm_new_message = 0;        // Cờ đã nhận xong frame hợp lệ

/**
 * @brief Đặt lại FSM về trạng thái ban đầu.
 *
 * Hàm này sẽ xóa bộ đếm, cờ trạng thái, dữ liệu đang lưu tạm, và đưa FSM
 * về trạng thái sẵn sàng nhận frame mới từ đầu.
 */
void Clear_All_State_Fsm(void)
{
    fsm_state = FSM_STATE_START;
    byte_count = 0;
    expected_payload_length = 0;
    flag_fsm_new_message = 0;
}

/**
 * @brief Nhận 1 byte dữ liệu từ UART và xử lý theo FSM để xây dựng frame.
 *
 * @param datain Byte dữ liệu đầu vào từ UART.
 * @param dataout Mảng dùng để lưu frame hợp lệ nếu nhận thành công.
 *
 * Nếu frame hợp lệ và đủ, dữ liệu sẽ được sao chép vào dataout và
 * FSM sẽ chuyển về trạng thái chờ frame mới.
 */
void Fsm_Get_Message(uint8_t datain, uint8_t dataout[])
{
    if (flag_fsm_new_message)
        return;


    switch (fsm_state)
    {
    case FSM_STATE_START:

        dataout[byte_count] = datain;

        if (byte_count == 0)
        {
            if (datain == START_BYTE)
            {

                Clear_All_State_Fsm();
                fsm_state = FSM_STATE_HEADER;
                byte_count++;
            }
            else
            {
                // Nếu byte nhận không phải START_BYTE, reset FSM
                Clear_All_State_Fsm();
            }
        }
        break;

    case FSM_STATE_HEADER:
        dataout[byte_count] = datain;
        byte_count++;
        // Check Group
        if (byte_count == 2)
        {

            if (datain != COMMAND)
            {
                Clear_All_State_Fsm();
            }
        }
        else if (byte_count == 3)
        {
            // Check ID
            if (dataout[2] != LDR && dataout[2] != PIR && dataout[2] != MQ2 && dataout[2] != HUMI && dataout[2] != TEMP &&
                dataout[2] != LED && dataout[2] != FAN && dataout[2] != BUZZER && dataout[2] != MODE && dataout[2] != UNKNOWN)
            {
                Clear_All_State_Fsm();
            }
        }
        else if (byte_count == 4)
        {
            // Check Length
            expected_payload_length = datain;
            if (expected_payload_length > PAYLOAD_MAX_SIZE)
            {
                Clear_All_State_Fsm();
            }
            else
            {
                fsm_state = FSM_STATE_PAYLOAD;
            }
        }

        break;

    case FSM_STATE_PAYLOAD:
        dataout[byte_count] = datain;
        byte_count++;
        if (byte_count == 1 + HEADER_SIZE + CHECKSUM_SIZE + expected_payload_length)
        {
            fsm_state = FSM_STATE_CHECKSUM;
        }
        else if (byte_count - 4 > expected_payload_length + 2)
        {

            // Nếu nhận quá payload, reset FSM
            Clear_All_State_Fsm();
        }
        break;

    case FSM_STATE_CHECKSUM:
        if (byte_count == 1 + HEADER_SIZE + expected_payload_length + CHECKSUM_SIZE)
        {

            uint16_t received_checksum = Convert_Bytes_To_Uint16(
                dataout[1 + HEADER_SIZE], dataout[1 + HEADER_SIZE + 1]);

            uint16_t calculated_checksum = Message_Calculate_Checksum(
                &dataout[1 + HEADER_SIZE + CHECKSUM_SIZE], expected_payload_length);

            if (received_checksum == calculated_checksum)
            {
                flag_fsm_new_message = 1;
            }
            else
            {

                // Nếu checksum không hợp lệ, reset FSM
                Clear_All_State_Fsm();
            }

            fsm_state = FSM_STATE_START;
        }

        break;
    }
}

/**
 * @brief Kiểm tra xem đã nhận được một frame hoàn chỉnh và hợp lệ chưa.
 *
 * @return 1 nếu đã nhận đúng và đủ frame; 0 nếu chưa đủ hoặc không hợp lệ.
 */
uint8_t Check_Fsm_Flag_New_Message(void)
{
    return flag_fsm_new_message;
}
