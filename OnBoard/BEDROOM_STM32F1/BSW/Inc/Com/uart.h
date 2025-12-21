/*
 * uart.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef INC_COM_UART_H_
#define INC_COM_UART_H_

#include <stm32_uart.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  Khởi tạo USART1 với tốc độ baud được chỉ định.
 * @param  baud_rate: tốc độ baud (ví dụ: 9600, 115200)
 */
void USART1_Init(uint32_t baud_rate);

/**
 * @brief  Gửi một ký tự qua USART1.
 * @param  chr: ký tự cần gửi
 */
void USART1_Send_Char(char chr);

/**
 * @brief  Gửi một chuỗi ký tự (null-terminated) qua USART1.
 * @param  str: con trỏ tới chuỗi cần gửi
 */
void USART1_Send_String(char* str);

/**
 * @brief  Gửi một mảng dữ liệu nhị phân qua USART1.
 * @param  data: con trỏ tới mảng dữ liệu
 * @param  length: số byte cần gửi
 */
void USART1_Send_Data(uint8_t* data, uint8_t length);

/**
 * @brief  Gửi một số nguyên kiểu int16_t dưới dạng chuỗi số qua USART1.
 * @param  num: số nguyên cần gửi
 */
void USART1_Send_Number(int16_t num);

/**
 * @brief  Gửi một số thực (float) dưới dạng chuỗi có định dạng qua USART1.
 * @param  num: số thực cần gửi
 */
void USART1_Send_Float(float num);

/**
 * @brief  Gửi một byte dữ liệu ở dạng thập lục phân (hex) dưới dạng ký tự qua USART1.
 * @param  num: giá trị 8-bit cần gửi
 */
void USART1_Send_Hex(uint8_t num);


StatusTypeDef DUNGX_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size);
StatusTypeDef DUNGX_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size);


#ifdef __cplusplus
}
#endif

#endif /* INC_COM_UART_H_ */
