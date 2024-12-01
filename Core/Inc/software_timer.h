/*
 * software_timer.h
 *
 *  Created on: Nov 27, 2024
 *      Author: than
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

void timer_init();
/**
 * Read ADC timer
 */
extern uint8_t timer_read_adc_flag;
void set_timer_read_adc(int delay);
void timer_read_adc_callback();

/**
 * Warning humidity data by sending through RS232 timer and ring buzzer
 */
extern uint8_t timer_warning_humidity_flag;
void set_timer_warning_humidity(int delay);
void timer_warning_humidity_callback();

/**
 * Display sensor value to LCD
 */
extern uint8_t timer_display_lcd_flag;
void set_timer_display_lcd(int delay);
void timer_display_lcd_callback();

/**
 * Update system seconds
 */
extern uint8_t timer_one_second_flag;
void set_timer_one_second(int delay);
void timer_one_second_callback();

/**
 * Scan led 7 segment
 */
extern uint8_t timer_scan_led_7seg_flag;
void set_timer_scan_led_7seg(int delay);
void timer_scan_led_7seg_callback();

/**
 * Send temp sensor value through UART2
 */
extern uint8_t timer_send_temp_sensor_flag;
void set_timer_send_temp_sensor(int delay);
void timer_send_temp_sensor_callback();

void timer_run();

#endif /* INC_SOFTWARE_TIMER_H_ */
