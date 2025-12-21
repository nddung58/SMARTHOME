/*
 * uart.c
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#include "../../../BSW/Inc/Com/uart.h"

#include "stm32_uart.h"
#include "stdio.h"

// Bộ xử lý UART toàn cục cho USART1
extern UART_HandleTypeDef huart1;

/**
 * @brief Khởi tạo USART1 với tốc độ baud được chỉ định.
 */
void USART1_Init(uint32_t baud_rate)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = baud_rate;
    // Word length 8-bit => M = 0 (bit 12 clear)
    huart1.Init.WordLength = 0;

    huart1.Init.StopBits = 0;                            // 1 stop bit (mặc định)
    huart1.Init.Parity = USART_CR1_PCE & ~USART_CR1_PCE; // No parity
    huart1.Init.Mode = UART_MODE_TX_RX;                  // TX + RX
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;         // Không sử dụng điều khiển luồng phần cứng

    UART_Init(&huart1); // Gọi hàm từ stm32_uart.c
}

/**
 * @brief Gửi một ký tự qua USART1.
 */
void USART1_Send_Char(char chr)
{
    UART_Transmit(&huart1, (uint8_t *)&chr, 1, 1000000);
}

/**
 * @brief Gửi một chuỗi null-terminated qua USART1.
 */
void USART1_Send_String(char *str)
{
    if (str != NULL)
    {
        UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 1000000);
    }
}

/**
 * @brief Gửi một mảng dữ liệu qua USART1.
 */
void USART1_Send_Data(uint8_t *data, uint8_t length)
{
    if (data != NULL && length > 0)
    {
        UART_Transmit(&huart1, data, length, 1000000);
    }
}

/**
 * @brief Gửi số nguyên dưới dạng chuỗi qua USART1.
 */
void USART1_Send_Number(int16_t num)
{
    char buffer[8];
    sprintf(buffer, "%d", num);
    USART1_Send_String(buffer);
}

/**
 * @brief Gửi số thực dưới dạng chuỗi qua USART1.
 */
void USART1_Send_Float(float num)
{
    char buffer[16];
    sprintf(buffer, "%.2f", num); // 2 chữ số sau dấu thập phân
    USART1_Send_String(buffer);
}

/**
 * @brief Gửi 1 byte dạng hex
 */
void USART1_Send_Hex(uint8_t num)
{
    char hexStr[3];
    sprintf(hexStr, "%02X", num);
    USART1_Send_String(hexStr);
}

/**
 * @brief Xử lý ngắt nhận dữ liệu từ USART1.
 * @note Hàm này sẽ được gọi tự động khi có dữ liệu đến USART1.
 */
void USART1_IRQHandler(void)
{
    UART_IRQHandler(&huart1);
}

StatusTypeDef DUNGX_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size)
{

    // Kiểm tra UART đã sẵn sàng nhận chưa
    if (huart->RxState == UART_STATE_READY)
    {

        if ((pData == NULL) || (Size == 0))
        {
            return DUNGX_ERROR; // Lỗi dữ liệu
        }

        // Cập nhật trạng thái và buffer nhận
        huart->RxState = UART_STATE_BUSY_RX;
        huart->ErrorCode = UART_ERROR_NONE;
        huart->RxXferSize = Size;
        huart->RxXferCount = Size;
        huart->pRxBuffPtr = pData;

        (void)huart1.Instance->DR; // Clear RXNE flag

        // Bật ngắt RXNE (nhận dữ liệu)
        huart->Instance->CR1 |= USART_CR1_RXNEIE;

        return DUNGX_OK; // Thành công
    }
    else
    {
        return DUNGX_BUSY; // UART đang bận
    }
}

StatusTypeDef DUNGX_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size)
{
    // Kiểm tra UART đã sẵn sàng gửi chưa
    if (huart->gState == UART_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0))
        {
            return DUNGX_ERROR; // Lỗi dữ liệu
        }

        // Cập nhật trạng thái và buffer gửi
        huart->gState = UART_STATE_BUSY_TX;
        huart->ErrorCode = UART_ERROR_NONE;
        huart->TxXferSize = Size;
        huart->TxXferCount = Size;
        huart->pTxBuffPtr = (uint8_t *)pData;

        // Bật ngắt TXE (gửi dữ liệu)
        huart->Instance->CR1 |= USART_CR1_TXEIE;

        return DUNGX_OK;
    }
    else
    {
        return DUNGX_BUSY;
    }
}
