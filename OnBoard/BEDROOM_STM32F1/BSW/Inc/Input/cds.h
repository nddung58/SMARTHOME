/*
 * cds.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef __CDS_H
#define __CDS_H

#include <stdint.h>
#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Khởi tạo cảm biến ánh sáng LDR sử dụng PA0 (ADC1 Channel 0).
     */
    void CDS_Init(void);

    /**
     * @brief Đọc giá trị ADC thô từ cảm biến LDR.
     * @return Giá trị ADC 12-bit (0–4095).
     */
    uint16_t CDS_ReadRaw(void);

    /**
     * @brief Chuyển đổi giá trị ADC sang lux.
     * @return Độ sáng đo được lux.
     */
    float CDS_ReadLux(void);

#ifdef __cplusplus
}
#endif

#endif // __CDS_H
