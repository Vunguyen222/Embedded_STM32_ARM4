/*
 * traffic_light_fsm.c
 *
 *  Created on: Nov 28, 2024
 *      Author: Lenovo
 */

#include "traffic_light_fsm.h"
#include "fsmc.h"
#include "lcd.h"
#include "global.h"

void Light_control_road1(int light) {
	switch (light) {
	case Red:
		lcd_DrawCircle(30, 100, RED , 15, 1); //red1
		lcd_DrawCircle(30, 150, WHITE, 15, 1); // yellow1
		lcd_DrawCircle(30, 150, YELLOW, 15, 0); // yellow1
		lcd_DrawCircle(30, 200, GREEN, 15, 0); // green1
		break;
	case Green:
		lcd_DrawCircle(30, 100, WHITE , 15, 1); //
		lcd_DrawCircle(30, 100, RED, 15, 0); // red1
		lcd_DrawCircle(30, 150, YELLOW, 15, 0); // yellow1
		lcd_DrawCircle(30, 200, GREEN, 15, 1); // green1
		break;
	case Yellow:
		lcd_DrawCircle(30, 100, RED, 15, 0); // red1
		lcd_DrawCircle(30, 150, YELLOW, 15, 1); // yellow1
		lcd_DrawCircle(30, 200, WHITE , 15, 1); //
		lcd_DrawCircle(30, 200, GREEN, 15, 0); // green1
		break;
	default:
		break;

	}
}

void Light_control_road2(int light) {
	switch(light) {
	case Red:
		lcd_DrawCircle(110, 60, RED, 15, 1); // red2
		lcd_DrawCircle(160, 60, WHITE, 15, 1); //
		lcd_DrawCircle(160, 60, YELLOW, 15, 0); // yellow2
		lcd_DrawCircle(210, 60, GREEN, 15, 0); // green2

		break;
	case Green:
		lcd_DrawCircle(110, 60, WHITE, 15, 1); //
		lcd_DrawCircle(110, 60, RED, 15, 0); // red2
		lcd_DrawCircle(160, 60, YELLOW, 15, 0); // yellow2
		lcd_DrawCircle(210, 60, GREEN, 15, 1); // green2
		break;
	case Yellow:
		lcd_DrawCircle(110, 60, RED, 15, 0); // red2
		lcd_DrawCircle(160, 60, YELLOW, 15, 1); // yellow2
		lcd_DrawCircle(210, 60, WHITE, 15, 1); //
		lcd_DrawCircle(210, 60, GREEN, 15, 0); // green2
		break;
	default:
		break;
	}
}
void Traffic_light_fsm1() {
	switch(Road1) {
		case Red:
			Light_control_road1(Red);
			count_red = (count_red + 1) % (time_red * 1000 / 50);
				if(count_red == 0) {
					Road1 = Green;
					Green1 = time_green;
				}

			break;
		case Green:
			Light_control_road1(Green);
			count_green = (count_green + 1) % (time_green * 1000 / 50);
				if(count_green == 0) {
					Road1 = Yellow;
					Yellow1 = time_yellow;
				}

			break;
		case Yellow:
			Light_control_road1(Yellow);
			count_yellow = (count_yellow + 1) % (time_yellow * 1000 / 50);
				if(count_yellow == 0) {
					Road1 = Red;
					Red1 = time_red;
				}

			break;
		default:
			break;
	}
}
void Traffic_light_fsm2() {
	switch(Road2) {
			case Red:
				Light_control_road2(Red);
				count_red = (count_red + 1) % (time_red * 1000 / 50);
				if(count_red == 0) {
					Road2 = Green;
					Green2 = time_green;
				}
				break;
			case Green:
				Light_control_road2(Green);
				count_green = (count_green + 1) % (time_green * 1000 / 50);
					if(count_red == 0) {
						Road2 = Yellow;
						Yellow2 = time_yellow;
					}
				break;
			case Yellow:
				Light_control_road2(Yellow);
				count_yellow = (count_yellow + 1) % (time_yellow * 1000 / 50);
					if(count_yellow == 0) {
						Road2 = Red;
						Red2  = time_red;
					}
				break;
			default:
				break;
		}
}

