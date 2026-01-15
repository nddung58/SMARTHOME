/*
 * interrupts.c
 *
 *  Created on: May 29, 2025
 *      Author: nguye
 */

#include "../../BSW/Inc/interrupts.h"

#include "stm32_gpio.h"
#include "stm32_adc.h"
#include "stm32_tim.h"
#include "stm32_uart.h"
#include "message.h"
#include "fsm_message.h"

#include "../../BSW/Inc/Com/fsm_message.h"
#include "../../BSW/Inc/Com/uart.h"
#include "../../BSW/Inc/globals.h"

#include "ndd_std_types.h"

#include "timer_base.h"

static message_t message;

uint8_t buffer[FRAME_MAX_SIZE];

void UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        for (int i = 0; i < 2; i++)
        {
            Fsm_Get_Message(uart_rx_buffer[i], buffer);

            if (Check_Fsm_Flag_New_Message())
            {

                if (Message_Decode(buffer, &message))
                {

                    push(&g_uartQueue, message, FRAME_MAX_SIZE);

                    // Gửi ACK
                    uint8_t data[FRAME_MAX_SIZE];
                    uint8_t res = RESPONSE_ACK;
                    uint8_t len = Create_Message(RESPONSE, UNKNOWN, 1, &res, data);
                    USART1_Send_Data(data, len);
                }
                else
                {
                    // Gửi NACK
                    uint8_t data[FRAME_MAX_SIZE];
                    uint8_t res = RESPONSE_NACK;
                    uint8_t len = Create_Message(RESPONSE, UNKNOWN, 1, &res, data);
                    USART1_Send_Data(data, len);
                }

                Clear_All_State_Fsm(); // Reset để nhận frame mới
            }
        }

        // Nhận byte tiếp theo
        DUNGX_UART_Receive_IT(&huart1, uart_rx_buffer, 2);
    }
}

void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Todo
}

