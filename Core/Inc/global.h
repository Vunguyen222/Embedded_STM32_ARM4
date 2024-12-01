/*
 * global.h
 *
 *  Created on: Nov 28, 2024
 *      Author: than
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_
#include <stdint.h>
#include "main.h"

extern uint16_t light_sensor_value;
extern uint16_t humidity_sensor_value;
extern float temperature_sensor_value;
extern float current_sensor_value;
extern float voltage_sensor_value;
extern uint16_t system_seconds;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim13;

extern SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#endif /* INC_GLOBAL_H_ */
