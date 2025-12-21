/*
 * motor.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <stdint.h>
#include "stm32_tim.h"

/**
 * @brief Khởi tạo motor.
 *
 * Cấu hình timer PWM, chân GPIO và chân điều khiển chiều quay.
 */
void Motor_Init(void);

/**
 * @brief Thiết lập tốc độ và chiều quay của motor.
 *
 * @param speed_percent Tốc độ motor (%), từ -100 đến 100. Giá trị âm là quay ngược.
 */
void Motor_SetSpeed(int8_t speed_percent);

/**
 * @brief Dừng motor (duty PWM = 0%).
 *
 * @param motor Con trỏ đến đối tượng motor.
 */
void Motor_Stop(void);

/**
 * @brief Thiết lập trạng thái của motor.
 *
 * @param state Trạng thái motor: 0 - Off, 1 - Low, 2 - Medium, 3 - High.
 */
void Motor_SetLevel(uint8_t level);

#endif /* __MOTOR_H__ */
