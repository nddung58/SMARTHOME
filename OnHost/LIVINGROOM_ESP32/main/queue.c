#include "queue.h"
#include <string.h>

void Queue_init(FrameQueue *q)
{
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

bool empty(const FrameQueue *q)
{
    return q->size == 0;
}

bool full(const FrameQueue *q)
{
    return q->size == FRAME_QUEUE_CAPACITY;
}

int size(const FrameQueue *q)
{
    return q->size;
}

bool push(FrameQueue *q, const message_t *frame)
{
    if (full(q))
        return false;
    if (frame == NULL)
        return false;

    q->rear = (q->rear + 1) % FRAME_QUEUE_CAPACITY;

    q->buffer[q->rear].frame = *frame;

    q->size++;
    return true;
}

bool pop(FrameQueue *q)
{
    if (empty(q))
        return false;

    q->front = (q->front + 1) % FRAME_QUEUE_CAPACITY;
    q->size--;
    return true;
}

bool front(FrameQueue *q, uint8_t *dest)
{
    if (q->size == 0 || dest == NULL)
        return false;

    memcpy(dest, q->buffer[q->front].data, FRAME_MAX_SIZE);
    return true;
}

bool back(FrameQueue *q, uint8_t *dest)
{
    if (q->size == 0 || dest == NULL)
        return false;

    int last_index = (q->rear == 0) ? (FRAME_QUEUE_CAPACITY - 1) : (q->rear - 1);
    memcpy(dest, q->buffer[last_index].data, FRAME_MAX_SIZE);
    return true;
}
