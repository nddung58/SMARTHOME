#include "message.h"
#include <string.h>

/**
 * @brief Kiểm tra dữ liệu
 */
uint16_t Message_Calculate_Checksum(const uint8_t *buf, uint8_t len)
{
    return 0xFFFF; // Todo: Implement checksum calculation
}

/**
 * @brief Tạo frame bản tin
 */
uint8_t Create_Message(Group_t g, id_end_device_t id, uint8_t length, const uint8_t *value, uint8_t *dataout)
{
    if (dataout == NULL)
        return 0;

    uint8_t count = 0;

    dataout[count++] = START_BYTE;
    dataout[count++] = g;
    dataout[count++] = id;
    dataout[count++] = length;

    uint16_t checksum = Message_Calculate_Checksum(value, length);
    // Write checksum in Big-Endian
    dataout[count++] = (checksum >> 8) & 0xFF; // High byte
    dataout[count++] = checksum & 0xFF;        // Low byte

    // Reverse byte order for Big-Endian
    for (int i = 0; i < length; i++)
    {
        dataout[count++] = value[i];
    }

    return count;
}

/**
 * @brief Giải mã frame nhận được
 */
bool Message_Decode(const uint8_t *buffer, message_t *frame_out)
{
    if (buffer == NULL || frame_out == NULL)
        return false;

    if (buffer[0] != START_BYTE)
        return false;

    frame_out->start = buffer[0];
    memcpy(frame_out->header, &buffer[1], HEADER_SIZE);
    uint8_t payload_len = frame_out->header[2];

    if (payload_len > PAYLOAD_MAX_SIZE)
        return false;

    uint16_t received_checksum = (buffer[1 + HEADER_SIZE] << 8) |
                                 (buffer[1 + HEADER_SIZE + 1]);

    memcpy(frame_out->payload, &buffer[1 + HEADER_SIZE + 2], payload_len);

    uint16_t calc_checksum = Message_Calculate_Checksum(&buffer[1 + HEADER_SIZE + 2], payload_len);

    if (received_checksum != calc_checksum)
        return false;

    frame_out->checksum = received_checksum;
    return true;
}
