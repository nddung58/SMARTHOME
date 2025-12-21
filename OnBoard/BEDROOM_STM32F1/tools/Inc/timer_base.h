#ifndef __TIMER_BASE_H__
#define __TIMER_BASE_H__

#include "stm32_tim.h"             

/**
 * @brief Khởi tạo timer nền (sử dụng TIM2) để tạo xung định kỳ 1ms.
 * 
 * Cần gọi hàm này một lần trong hàm main() để bắt đầu hệ thống đếm thời gian.
 */
void Timer_Init(void);          

/**
 * @brief Hàm tăng biến Tick, được gọi trong ngắt Timer 1ms.
 * 
 * Hàm này nên được gọi trong ISR của TIM2 (trong hàm `TIM2_IRQHandler`)
 * để cập nhật thời gian hệ thống.
 */
void IncTick(void);       

/**
 * @brief Lấy thời gian hiện tại tính bằng mili giây kể từ lúc khởi động Timer.
 * 
 * @return Giá trị thời gian (ms).
 */
uint32_t GetTick(void);       

/**
 * @brief Tạo độ trễ bằng mili giây (ms).
 * 
 * Sử dụng vòng lặp chờ chủ động (blocking delay).
 * @param ms Số mili giây cần delay.
 */
void Delay_ms(uint32_t ms);  

/**
 * @brief Tạo độ trễ bằng micro giây (us).
 * 
 * Yêu cầu hệ thống có DWT hoặc bộ đếm tốc độ cao để đảm bảo chính xác.
 * @param us Số micro giây cần delay.
 */
void Delay_us(uint32_t us);

#endif /* __TIMER_BASE_H__ */
