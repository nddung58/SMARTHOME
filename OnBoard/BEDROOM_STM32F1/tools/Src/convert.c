/*
 * convert.c
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#include "convert.h"
#include <stdio.h>
#include "stdint.h"

/*
 * @brief: Chuyển đổi một biến float sang dạng mảng byte (4 byte).
 * @param data: Biến kiểu float cần chuyển đổi.
 * @return: Con trỏ trỏ đến mảng byte tương ứng.
 */
uint8_t *Convert_Float_To_Bytes(float data)
{
    static data_convert_float_and_bytes input;
    input.data_float = data;
    return input.data_bytes;
}

/*
 * @brief: Chuyển đổi một biến uint16_t sang mảng byte (2 byte).
 * @param data: Biến kiểu uint16_t cần chuyển đổi.
 * @return: Con trỏ trỏ đến mảng byte tương ứng.
 */
uint8_t *Convert_Uint16_To_Bytes(uint16_t data)
{
    static data_convert_uint16_and_bytes input;
    input.data_uint16 = data;
    return input.data_bytes;
}

/*
 * @brief: Chuyển 4 byte riêng lẻ thành biến float.
 * @param data1: Byte thứ nhất.
 * @param data2: Byte thứ hai.
 * @param data3: Byte thứ ba.
 * @param data4: Byte thứ tư.
 * @return: Biến float sau khi ghép.
 */
float Convert_Bytes_To_Float(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    data_convert_float_and_bytes input;
    input.data_bytes[0] = data1;
    input.data_bytes[1] = data2;
    input.data_bytes[2] = data3;
    input.data_bytes[3] = data4;
    return input.data_float;
}

int Convert_Bytes_To_Int(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    data_convert_int_and_bytes input;
    input.data_bytes[0] = data1;
    input.data_bytes[1] = data2;
    input.data_bytes[2] = data3;
    input.data_bytes[3] = data4;
    return input.data_int;
}

/*
 * @brief: Chuyển 2 byte thành biến uint16_t.
 * @param data1: Byte thứ nhất.
 * @param data2: Byte thứ hai.
 * @return: Biến uint16_t sau khi ghép từ 2 byte.
 */
uint16_t Convert_Bytes_To_Uint16(uint8_t data1, uint8_t data2)
{
    data_convert_uint16_and_bytes input;
    input.data_bytes[0] = data1;
    input.data_bytes[1] = data2;
    return input.data_uint16;
}
