/*
 * global.h
 *
 *  Created on: Nov 28, 2024
 *      Author: Lenovo
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

enum State { INIT, STATE_NORMAL, STATE_RED, STATE_YELLOW,
	STATE_GREEN
};


enum TrafficLight { Green, Yellow, Red };
// state
extern int status;
// duration led
extern int temp_duration_red;
extern int temp_duration_yellow;
extern int temp_duration_green;

extern int blinky_count_led_debug;
extern int blinky_count_led_Y0;
extern int blinkly_count_led_Y1;

extern int count_red;
extern int count_green;
extern int count_yellow;
// road
extern int Road1;
extern int Road2;
// timer counter
extern int time_red;
extern int time_yellow;
extern int time_green;

// led
extern int Red1;
extern int Green1;
extern int Yellow1;

extern int Red2;
extern int Green2;
extern int Yellow2;



#endif /* INC_GLOBAL_H_ */
