/*
 * global.c
 *
 *  Created on: Nov 28, 2024
 *      Author: Lenovo
 */


#include "global.h"


int time_red = 10;
int time_green= 5;
int time_yellow = 2;

int temp_duration_red = 1;
int temp_duration_yellow = 1;
int temp_duration_green = 1;

int status = INIT;

int blinky_count_led_debug = 0;
int blinky_count_led_Y0 = 0;
int blinkly_count_led_Y1 = 0;

// load light
int count_red = 0;
int count_green = 0;
int count_yellow = 0;

// load road
int Road1;
int Road2;

int Red1, Red2;
int Green1, Green2;
int Yellow1, Yellow2;
