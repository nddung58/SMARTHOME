#ifndef INC_MESSAGE_H
#define INC_MESSAGE_H

#include <convert.h>
#include <stdbool.h>
#include <stdint.h>
#include "ndd_std_types.h"

#define START_BYTE 0xAA
#define HEADER_SIZE 3
#define PAYLOAD_MAX_SIZE 10
#define CHECKSUM_SIZE 2
#define FRAME_MAX_SIZE 16

// Cấu trúc frame dữ liệu
typedef struct
{
    uint8_t start;               // 1 byte: START_BYTE = 0xAA
    uint8_t header[HEADER_SIZE]; // 3 byte: GROUP (1Byte) + ID (1Byte) + Length(1Byte)
    uint16_t checksum;
    uint8_t payload[PAYLOAD_MAX_SIZE]; // N byte: dữ liệu

} message_t;

typedef enum
{
    COMMAND = 0X01,
    RESPONSE = 0X02,
    NOTIFY = 0X03
} Group_t;

typedef enum
{
    RESPONSE_ACK = 0x01,
    RESPONSE_NACK = 0x02,
    RESPONSE_BUSY = 0x03,
    RESPONSE_READY = 0x04,
} RESPONSE_t;

// ==== Prototype của các hàm xử lý frame ====

void Create_Message(Group_t g, id_end_device_t id, uint8_t length, const uint8_t *value, message_t *messageout);

/**
 * @brief Phân tích frame đầu vào từ buffer
 * @param buffer: dữ liệu thô
 * @param frame_out: nơi lưu kết quả
 * @return true nếu hợp lệ
 */
bool Message_Decode(const uint8_t *buffer, message_t *dataout);

/**
 * @brief Tính tổng kiểm tra
 * @param buf: buffer
 * @param len: độ dài tính
 * @return giá trị checksum (2 byte)
 */
uint16_t Message_Calculate_Checksum(const uint8_t *buf, uint8_t len);

bool Check_Frame_Header(const uint8_t headerframe[]);

#endif
