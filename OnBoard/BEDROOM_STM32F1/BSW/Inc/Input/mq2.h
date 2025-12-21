/*
 * mq2.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef INC_MQ2_H_
#define INC_MQ2_H_

#include <stdint.h>

/**
 * @brief Khởi tạo cảm biến MQ2 với giá trị điện trở Ro trong không khí sạch.
 * 
 * Hàm này thiết lập giá trị chuẩn ban đầu của cảm biến MQ2 (Ro) trong môi trường không khí sạch.
 * Ro là điện trở cảm biến khi không có khí cháy, và được dùng để tính toán tỷ lệ R/Ro.
 * 
 * @param ro_clean_air Giá trị Ro (điện trở cảm biến trong không khí sạch), thường từ 9kΩ đến 10kΩ.
 */
void MQ2_Init(void);

/**
 * @brief Đọc giá trị ADC từ cảm biến MQ2.
 * Hàm này đọc giá trị ADC từ cảm biến MQ2 và trả về giá trị thô (raw) dưới dạng uint16_t.
 */
uint16_t MQ2_ReadRaw(void);

/**
 * @brief Đọc và tính toán chất lượng không khí từ cảm biến MQ2.
 * 
 * Dựa trên giá trị ADC hiện tại và Ro, hàm này trả về chất lượng không khí 
 * dưới dạng tỷ lệ R/Ro hoặc ppm tuỳ theo cách bạn xây dựng hàm bên trong `mq2.c`.
 * 
 * @return Giá trị chất lượng không khí (float). Giá trị càng cao → không khí càng ô nhiễm.
 */
float MQ2_ReadLevel(void);

#endif /* INC_MQ2_H_ */
