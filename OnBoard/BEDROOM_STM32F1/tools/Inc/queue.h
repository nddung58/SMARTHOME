/*
 * queue
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include "message.h"

// Kích thước tối đa (byte) của một khung dữ liệu
#define FRAME_SIZE 10

// Sức chứa tối đa của hàng đợi (số lượng khung)
#define FRAME_QUEUE_CAPACITY 20

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cấu trúc hàng đợi vòng lưu các khung dữ liệu kiểu message_t
 */
typedef struct {
    message_t buffer[FRAME_QUEUE_CAPACITY];  ///< Bộ đệm chứa các khung
    int front;     ///< Vị trí đầu hàng đợi (khung sắp được lấy ra)
    int rear;      ///< Vị trí cuối hàng đợi (khung mới được thêm vào)
    int size;      ///< Số lượng phần tử hiện có trong hàng đợi
} FrameQueue;

/**
 * @brief Khởi tạo hàng đợi về trạng thái rỗng
 * 
 * @param q Con trỏ trỏ tới hàng đợi cần khởi tạo
 */
void Queue_init(FrameQueue *q);

/**
 * @brief Thêm một khung dữ liệu vào cuối hàng đợi (enqueue)
 * 
 * @param q Con trỏ tới hàng đợi
 * @param f Khung dữ liệu cần thêm vào
 * @param length Độ dài dữ liệu hợp lệ trong khung (nếu cần)
 * @return true nếu thêm thành công, false nếu hàng đợi đầy
 */
bool push(FrameQueue *q, const message_t f, uint8_t length);

/**
 * @brief Xóa một khung dữ liệu ở đầu hàng đợi (dequeue)
 * 
 * @param q Con trỏ tới hàng đợi
 * @return true nếu xóa thành công, false nếu hàng đợi rỗng
 */
bool pop(FrameQueue *q);

/**
 * @brief Truy xuất khung dữ liệu ở đầu hàng đợi (không xóa)
 * 
 * @param q Con trỏ tới hàng đợi
 * @return Con trỏ tới khung đầu tiên; NULL nếu hàng đợi rỗng
 */
message_t* front(const FrameQueue *q);

/**
 * @brief Truy xuất khung dữ liệu được thêm gần nhất (ở cuối)
 * 
 * @param q Con trỏ tới hàng đợi
 * @return Con trỏ tới khung cuối; NULL nếu hàng đợi rỗng
 */
message_t* back(const FrameQueue *q);

/**
 * @brief Kiểm tra xem hàng đợi có rỗng không
 * 
 * @param q Con trỏ tới hàng đợi
 * @return true nếu rỗng, false nếu có phần tử
 */
bool empty(const FrameQueue *q);

/**
 * @brief Kiểm tra xem hàng đợi đã đầy chưa
 * 
 * @param q Con trỏ tới hàng đợi
 * @return true nếu đầy, false nếu còn chỗ
 */
bool full(const FrameQueue *q);

/**
 * @brief Trả về số lượng phần tử hiện có trong hàng đợi
 * 
 * @param q Con trỏ tới hàng đợi
 * @return Số phần tử hiện có trong hàng đợi
 */
int size(const FrameQueue *q);

#ifdef __cplusplus
}
#endif

#endif  // _QUEUE_H
