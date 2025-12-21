#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include "message.h"

#define FRAME_QUEUE_CAPACITY 20

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Union dùng để truy cập khung dữ liệu dưới dạng cấu trúc hoặc mảng byte.
     */
    typedef union
    {
        message_t frame;
        uint8_t data[FRAME_MAX_SIZE];
    } Message_Convert_t;

    /**
     * @brief Cấu trúc hàng đợi dùng để lưu trữ các khung dữ liệu.
     */
    typedef struct
    {
        Message_Convert_t buffer[FRAME_QUEUE_CAPACITY];
        int front;
        int rear;
        int size;
    } FrameQueue;

    void Queue_init(FrameQueue *q);

    bool push(FrameQueue *q, const message_t *frame);

    bool pop(FrameQueue *q);

    bool front(FrameQueue *q, uint8_t *dest);

    bool back(FrameQueue *q, uint8_t *dest);

    bool empty(const FrameQueue *q);

    bool full(const FrameQueue *q);

    int size(const FrameQueue *q);

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_H */
