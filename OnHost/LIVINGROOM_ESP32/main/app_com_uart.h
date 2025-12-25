#ifndef APP_COM_UART_H__
#define APP_COM_UART_H__

#include "convert.h"
#include "fsm_message.h"
#include "queue.h"

void COM_UART_Init(uint32_t baud_rate);

void uart_tx_task(void *param);

void uart_event_task(void *pvParameters);

void COM_HandleNotifyMessage(void);

void COM_HandleResponseMessage(FrameQueue *queue);
uint8_t COM_Get_Bedroom_Mode(void);

#endif // APP_COM_UART_H__
