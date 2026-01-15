/*
 * interrupts.h
 *
 *  Created on: May 29, 2025
 *      Author: nguye
 */

#ifndef INC_COM_INTERRUPTS_H_
#define INC_COM_INTERRUPTS_H_

#define PIR_PIN GPIO_PIN_5

/**
 * @brief Đặt lại toàn bộ trạng thái nhận dữ liệu về trạng thái ban đầu.
 *
 * Được gọi khi phát hiện lỗi hoặc sau khi xử lý xong 1 khung dữ liệu.
 */
void Clear_All_State(void);

#endif /* INC_COM_INTERRUPTS_H_ */
