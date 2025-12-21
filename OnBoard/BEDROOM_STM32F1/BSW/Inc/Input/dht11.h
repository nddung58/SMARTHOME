/*
 * dht11.h
 *
 *  Created on: May 22, 2025
 *      Author: nguye
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32_gpio.h"
#include <stdbool.h>

#define DHT11_PORT   GPIOA
#define DHT11_PIN    (1U << 2)  // PA2

/**
 * @brief Initialize the DHT11 sensor.
 */
void DHT11_Init(void);


/**
 * @brief Read temperature and humidity from the DHT11 sensor.
 * 
 * @param temperature Pointer to store the temperature value in Celsius.
 * @param humidity Pointer to store the humidity value in percentage.
 * @return true if the read was successful, false otherwise.
 */
bool DHT11_Read(uint8_t *humi_int, uint8_t *humi_dec, uint8_t *temp_int, uint8_t *temp_dec);




#endif /* INC_DHT11_H_ */
