/*
 * software_timer.c
 *
 *  Created on: Nov 27, 2024
 *      Author: than
 */



#include <stdio.h>
#include "global.h"
#include "software_timer.h"
#include "sensor.h"
#include "uart.h"
#include "buzzer.h"
#include "led_7seg.h"
#include "lcd.h"

void timer_init(){
	HAL_TIM_Base_Start_IT(&htim2);
}


/**
 * Read ADC timer
 */
uint8_t timer_read_adc_flag = 0;
int timer_read_adc_count = 0;
void set_timer_read_adc(int delay)
{
	timer_read_adc_flag = 0;
	timer_read_adc_count = delay;
}
void timer_read_adc_callback()
{
	sensor_Read();
	light_sensor_value = sensor_GetLight();

	humidity_sensor_value = ((float)sensor_GetPotentiometer() / 4095.0) * 100;
	if(humidity_sensor_value > 70)
	{
		set_timer_warning_humidity(1);
	}

	temperature_sensor_value = sensor_GetTemperature();
	current_sensor_value = sensor_GetCurrent();
	voltage_sensor_value = sensor_GetVoltage();

//	uint8_t buffer[100];
//	uint16_t length = sprintf(buffer, "!%d,%d,%d,%d,%d#",
//			light_sensor_value, humidity_sensor_value, temperature_sensor_value,
//			current_sensor_value, voltage_sensor_value);
//	uart_Rs232SendBytes(buffer, length);
}

/**
 * Warning humidity data by sending through RS232 timer and ring buzzer
 */
uint8_t timer_warning_humidity_flag = 0;
int timer_warning_humidity_count = 0;
void set_timer_warning_humidity(int delay)
{
	timer_warning_humidity_flag = 0;
	timer_warning_humidity_count = delay;
}

static uint8_t send_warning_buf[30];
static uint8_t buzzer_status = 0;
void timer_warning_humidity_callback()
{
	if(humidity_sensor_value > 70)
	{
		/* Send data through RS232 */
		uint16_t length = sprintf(send_warning_buf, "!%d#", humidity_sensor_value);
		uart_Rs232SendBytes(send_warning_buf, length);

		/* Ring buzzer */
		if(buzzer_status) {
			buzzer_status = 0;
			buzzer_SetVolume(0);
		}else{
			buzzer_status = 1;
			buzzer_SetVolume(50);
		}
	}else{
		/* Turn off buzzer */
		buzzer_status = 0;
		buzzer_SetVolume(0);
	}
}


/**
 * Display sensor value to LCD
 */
uint8_t timer_display_lcd_flag = 0;
int timer_display_lcd_count = 0;
void set_timer_display_lcd(int delay)
{
	timer_display_lcd_flag = 0;
	timer_display_lcd_count = delay;
}

void timer_display_lcd_callback()
{
	lcd_ShowStr(10, 100, "Power:", RED, BLACK, 16, 0);
	lcd_ShowFloatNum(130, 100, voltage_sensor_value * current_sensor_value, 4, RED, BLACK, 16);
	lcd_ShowStr(10, 120, "Light:", RED, BLACK, 16, 0);
	lcd_ShowIntNum(130, 120, light_sensor_value, 4, RED, BLACK, 16);
	lcd_ShowStr(10, 140, "Temperature:", RED, BLACK, 16, 0);
	lcd_ShowFloatNum(130, 140, temperature_sensor_value, 4, RED, BLACK, 16);
	lcd_ShowStr(10, 160, "Humidity:", RED, BLACK, 16, 0);
	lcd_ShowIntNum(130, 160, humidity_sensor_value, 4 , RED, BLACK, 16);
}

/**
 * Update system seconds
 */
uint8_t timer_one_second_flag = 0;
int timer_one_second_count = 0;
void set_timer_one_second(int delay)
{
	timer_one_second_flag = 0;
	timer_one_second_count = delay;
}
void timer_one_second_callback()
{
	system_seconds = (system_seconds + 1) % 10000;

	/* Set led 7 value */
	uint16_t system_seconds_temp = system_seconds;
	uint8_t digit = 0;
	for(uint8_t i = 0; i < 4; i++)
	{
		digit = system_seconds_temp % 10;
		system_seconds_temp /= 10;
		led7_SetDigit(digit, 3 - i, 0);
	}

	HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);


}

/**
 * Scan led 7 segment
 */
uint8_t timer_scan_led_7seg_flag = 0;
int timer_scan_led_7seg_count = 0;
void set_timer_scan_led_7seg(int delay)
{
	timer_scan_led_7seg_flag = 0;
	timer_scan_led_7seg_count = delay;
}
void timer_scan_led_7seg_callback()
{
	led7_Scan();
}


/**
 * Send temp sensor value through UART2
 */
uint8_t timer_send_temp_sensor_flag = 0;
int timer_send_temp_sensor_count = 0;
void set_timer_send_temp_sensor(int delay)
{
	timer_send_temp_sensor_flag = 0;
	timer_send_temp_sensor_count = delay;
}
void timer_send_temp_sensor_callback()
{
	uint8_t buffer[30];
	uint16_t length = sprintf(buffer, "!%f#", temperature_sensor_value);
	uart2_SendBytes(buffer, length);
//	uart2_SendBytes("!53#", 4);
}


/* ------------- */
void timer_run()
{
	/**
	 * Read ADC timer
	 */
	if(timer_read_adc_count > 0)
	{
		timer_read_adc_count --;
		if(timer_read_adc_count <= 0)
		{
			timer_read_adc_flag = 1;
		}
	}

	/**
	 * Warning data by sending through RS232 and ring buzzer
	 */
	if(timer_warning_humidity_count > 0)
	{
		timer_warning_humidity_count --;
		if(timer_warning_humidity_count <= 0)
		{
			timer_warning_humidity_flag = 1;
		}
	}

	/**
	 * Display sensor value to LCD
	 */
	if(timer_display_lcd_count > 0)
	{
		timer_display_lcd_count --;
		if(timer_display_lcd_count <= 0)
		{
			timer_display_lcd_flag = 1;
		}
	}

	/**
	 * Update system seconds
	 */
	if(timer_one_second_count > 0)
	{
		timer_one_second_count --;
		if(timer_one_second_count <= 0)
		{
			timer_one_second_flag = 1;
		}
	}

	/**
	 * Scan led 7 segment
	 */
	if(timer_scan_led_7seg_count > 0)
	{
		timer_scan_led_7seg_count --;
		if(timer_scan_led_7seg_count <= 0)
		{
			timer_scan_led_7seg_flag = 1;
		}
	}

	/**
	 * Send temp sensor value through UART2
	 */
	if(timer_send_temp_sensor_count> 0)
	{
		timer_send_temp_sensor_count --;
		if(timer_send_temp_sensor_count <= 0)
		{
			timer_send_temp_sensor_flag = 1;
		}
	}

}



