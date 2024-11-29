/*
 * system_fsm.c
 *
 *  Created on: Nov 28, 2024
 *      Author: Lenovo
 */


#include "system_fsm.h"
#include "button.h"
#include "software_timer.h"
#include "lcd.h"
#include "global.h"
void system_fsm() {
	switch(status){
	case INIT:
		if(button_count[0] == 1) {
			status = STATE_NORMAL;
			// set default status
			Road1 = Red;
			Road2 = Green;
			// set default time
			Red1 = time_red;
			Green2 = time_green;

		}
		break;
	case STATE_NORMAL:
		lcd_StrCenter(-20, 80, "START SYSTEM", WHITE, WHITE, 24, 1);
		lcd_ShowStr(10, 220, "Road1", BLACK, WHITE, 16, 0);
		lcd_ShowStr(140, 80, "Road2", BLACK, WHITE, 16, 0);
		Traffic_light_fsm1();
		Traffic_light_fsm2();

		// display mode

		lcd_ShowStr(90, 120, "Mode: ", BLACK, WHITE, 24, 0);
		lcd_ShowIntNum(150, 120, 1 , 2, BLACK, WHITE, 24);
		lcd_ShowStr(180, 120, "N", BLACK, WHITE, 24, 0);
		// hidden duration
		lcd_ShowStr(90, 150, "Duration: ",WHITE, WHITE, 24, 0);
		lcd_ShowIntNum(200, 150, 0 , 2, WHITE, WHITE, 24);
		lcd_ShowIntNum(202, 150, 0 , 2, WHITE, WHITE, 24);
		if(button_count[0] == 1) {
			//button_count[0] = 0;
//			setTimer3(500);
			status = STATE_RED;
		}
		break;
	case STATE_RED:
		// blink led 2HZ
		blinky_count_led_debug = (blinky_count_led_debug + 1)%10;
			if(blinky_count_led_debug == 0){
				HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
			}
			HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
			HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
		//button1
		if(button_count[0] == 1) {
				status = STATE_YELLOW;
			}
		// display Mode
		lcd_ShowStr(90, 120, "Mode: ", BLACK, WHITE, 24, 0);
		lcd_ShowIntNum(150, 120, 2 , 2, BLACK, WHITE, 24);

		lcd_ShowStr(180, 120, "R", BLACK, WHITE, 24, 0);

		// display duration
		lcd_ShowStr(90, 150, "Duration: ", BLACK, WHITE, 24, 0);
		//button 2
		lcd_ShowIntNum(200, 150, temp_duration_red , 2, BLACK, WHITE, 24);
		if(button_count[1] == 1) {
			temp_duration_red++;
			lcd_ShowIntNum(200, 150, temp_duration_red , 2, BLACK, WHITE, 24);
			if(temp_duration_red > 99) {
				temp_duration_red = 1;
			}
		}

		//button3
		if(button_count[2] == 1) {
			time_red = temp_duration_red;
			time_green = temp_duration_green;
			time_yellow = temp_duration_yellow;
			temp_duration_red = 1;
			status = STATE_NORMAL;
		}
		break;
	case STATE_YELLOW:

		// blink led 2HZ
		blinky_count_led_Y0 = (blinky_count_led_Y0 + 1)%10;
			if(blinky_count_led_Y0 == 0){
				HAL_GPIO_TogglePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin);
			}
			HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
			HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);

		// button 1
		if(button_count[0] == 1) {
			status = STATE_GREEN;
		}

		// display Mode
		lcd_ShowStr(90, 120, "Mode: ", BLACK, WHITE, 24, 0);
		lcd_ShowIntNum(150, 120, 3 , 2, BLACK, WHITE, 24);

		lcd_ShowStr(180, 120, "Y", BLACK, WHITE, 24, 0);

		// display duration
		lcd_ShowStr(90, 150, "Duration: ", BLACK, WHITE, 24, 0);
		//button2
		lcd_ShowIntNum(200, 150, temp_duration_yellow , 2, BLACK, WHITE, 24);
		if(button_count[1] == 1) {
			temp_duration_yellow++;
			lcd_ShowIntNum(200, 150, temp_duration_yellow , 2, BLACK, WHITE, 24);
			if(temp_duration_yellow > 99) {
				temp_duration_yellow = 1;
			}
		}
		// button 3
		if(button_count[2] == 1) {
			time_red = temp_duration_red;
			time_green = temp_duration_green;
			time_yellow = temp_duration_yellow;
			temp_duration_yellow = 1;
			status = STATE_NORMAL;
		}
		break;
	case STATE_GREEN:

		// blink led 2HZ
		blinkly_count_led_Y1 = (blinkly_count_led_Y1 + 1)%10;
			if(blinkly_count_led_Y1 == 0){
				HAL_GPIO_TogglePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin);
			}
			HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
			HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);


		// display Mode
		lcd_ShowStr(90, 120, "Mode: ", BLACK, WHITE, 24, 0);
		lcd_ShowIntNum(150, 120, 4 , 2, BLACK, WHITE, 24);

		lcd_ShowStr(180, 120, "G", BLACK, WHITE, 24, 0);
		// display duration
		lcd_ShowStr(90, 150, "Duration: ", BLACK, WHITE, 24, 0);
		// Button 1
		if(button_count[0] == 1) {
				status = STATE_NORMAL;
			}

		// button 2
		lcd_ShowIntNum(200, 150, temp_duration_green , 2, BLACK, WHITE, 24);
		if(button_count[1] == 1) {
			temp_duration_green++;
			lcd_ShowIntNum(200, 150, temp_duration_green , 2, BLACK, WHITE, 24);
			if(temp_duration_green > 99) {
				temp_duration_green = 1;
			}
		}
		// button 3
		if(button_count[2] == 1) {
			time_red = temp_duration_red;
			time_green = temp_duration_green;
			time_yellow = temp_duration_yellow;
			temp_duration_green = 1;
			status = STATE_NORMAL;
		}
		break;
	default:
		break;
	}
}
