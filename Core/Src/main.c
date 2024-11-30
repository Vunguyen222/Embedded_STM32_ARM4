/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "lcd.h"
#include "ds3231.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INIT_SYSTEM     0
#define SET_HOUR        1
#define SET_MINUTE      2
#define SET_DAY         3
#define SET_DATE        4
#define SET_MONTH       5
#define SET_YEAR        6
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t count_modify = 0;
uint8_t count_alarm = 0;
uint8_t temp = 0;
uint8_t temp_alarm = 0;
uint8_t flag = 0;
enum Mode {
	modeInit,
	modeNormal,
	modeModify,
	modeAlarm,
};
enum Mode currentMode = modeInit;
uint8_t temp_time[7];
uint8_t alarm[7];
uint8_t alarmPresent[7];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void displayTime();
void updateTime();
unsigned char isButtonUp();
unsigned char isButtonDown();
char* Mode();
void checkTime();
void checkAlarm();
void modifyTime();
void modifyAlarm();
void fsm();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_SPI1_Init();
	MX_FSMC_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */
	system_init();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	lcd_Clear(BLACK);
	updateTime();

	while (1) {
		while (!flag_timer2);
		flag_timer2 = 0;
		button_Scan();
		ds3231_ReadTime();
		fsm();
    /* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void system_init() {
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	timer_init();
	button_init();
	lcd_init();
	ds3231_init();
	setTimer2(50);
}

void updateTime(){
	ds3231_Write(ADDRESS_YEAR, 24);
	ds3231_Write(ADDRESS_MONTH, 11);
	ds3231_Write(ADDRESS_DATE, 23);
	ds3231_Write(ADDRESS_DAY, 2);
	ds3231_Write(ADDRESS_HOUR, 9);
	ds3231_Write(ADDRESS_MIN, 35);
	ds3231_Write(ADDRESS_SEC, 30);
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}
uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}
char* Mode() {
	switch (currentMode) {
		case modeInit:
			return "Initial";
			break;
		case modeNormal:
			return "Normal";
			break;
		case modeModify:
			return "Modify";
			break;
		case modeAlarm:
			return "Alarm";
			break;
	}
	return "Unknown!";
}

void checkTime() {
	switch (temp) {
		case 0:
			if (temp_time[temp] > 24) {
				temp_time[temp] = 0;
			}
			break;
		case 1:
			if (temp_time[temp] > 59) {
				temp_time[temp] = 0;
			}
			break;
		case 2:
			if (temp_time[temp] > 59) {
				temp_time[temp] = 0;
			}
			break;
		case 3:
			if (temp_time[temp] > 8) {
				temp_time[temp] = 2;
			}
			break;
		case 4:
			if (temp_time[temp] > 31) {
				temp_time[temp] = 0;
			}
			break;
		case 5:
			if (temp_time[temp] > 12) {
				temp_time[temp] = 0;
			}
			break;
		case 6:
			if (temp_time[temp] > 99) {
				temp_time[temp] = 0;
			}
			break;
	}
}

void checkAlarm() {
	switch (temp_alarm) {
		case 0:
			if (alarm[temp_alarm] > 24) {
				alarm[temp_alarm] = 0;
			}
			break;
		case 1:
			if (alarm[temp_alarm] > 59) {
				alarm[temp_alarm] = 0;
			}
			break;
		case 2:
			if (alarm[temp_alarm] > 59) {
				alarm[temp_alarm] = 0;
			}
			break;
		case 3:
			if (alarm[temp_alarm] > 8) {
				alarm[temp_alarm] = 2;
			}
			break;
		case 4:
			if (alarm[temp_alarm] > 31) {
				alarm[temp_alarm] = 0;
			}
			break;
		case 5:
			if (alarm[temp_alarm] > 12) {
				alarm[temp_alarm] = 0;
			}
			break;
		case 6:
			if (alarm[temp_alarm] > 99) {
				alarm[temp_alarm] = 0;
			}
			break;
	}
}

void displayTime(){
	lcd_StrCenter(0, 50, Mode(), WHITE, BLACK, 24, 0);
	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowChar(100, 100, ':', GREEN, BLACK, 24, 0);
	lcd_ShowIntNum(120, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowChar(150, 100, ':', GREEN, BLACK, 24, 0);
	lcd_ShowIntNum(170, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(120, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(170, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}

void modifyTime() {
	count_modify = (count_modify + 1) % 10;
	if (count_modify > 5) {
		switch (temp){
			case 0:
				lcd_ShowIntNum(70, 100, "  ", 2, BLACK, BLACK, 24);
				break;
			case 1:
				lcd_ShowIntNum(120, 100, "  ", 2, BLACK, BLACK, 24);
				break;
			case 2:
				lcd_ShowIntNum(170, 100, "  ", 2, BLACK, BLACK, 24);
				break;
			case 3:
				lcd_ShowIntNum(20, 130, "  ", 2, BLACK, BLACK, 24);
				break;
			case 4:
				lcd_ShowIntNum(70, 130, "  ", 2, BLACK, BLACK, 24);
				break;
			case 5:
				lcd_ShowIntNum(120, 130, "  ", 2, BLACK, BLACK, 24);
				break;
			case 6:
				lcd_ShowIntNum(170, 130, "  ", 2, BLACK, BLACK, 24);
				break;
		}
	} else {
		lcd_StrCenter(0, 50, Mode(), WHITE, BLACK, 24, 0);
		lcd_ShowIntNum(70, 100, temp_time[0], 2, GREEN, BLACK, 24);
		lcd_ShowIntNum(120, 100, temp_time[1], 2, GREEN, BLACK, 24);
		lcd_ShowIntNum(170, 100, temp_time[2], 2, GREEN, BLACK, 24);
		lcd_ShowIntNum(20, 130, temp_time[3], 2, YELLOW, BLACK, 24);
		lcd_ShowIntNum(70, 130, temp_time[4], 2, YELLOW, BLACK, 24);
		lcd_ShowIntNum(120, 130, temp_time[5], 2, YELLOW, BLACK, 24);
		lcd_ShowIntNum(170, 130, temp_time[6], 2, YELLOW, BLACK, 24);
	}

	if (button_count[3] == 1) {
		temp_time[temp]++;
		checkTime();
	}

	if (button_count[3] > 40 && button_count[3] % 4 == 1) {
		temp_time[temp]++;
		checkTime();
	}

	if (button_count[12] == 1) {
		switch (temp){
			case 0:
				ds3231_Write(ADDRESS_HOUR, temp_time[temp]);
				break;
			case 1:
				ds3231_Write(ADDRESS_MIN, temp_time[temp]);
				break;
			case 2:
				ds3231_Write(ADDRESS_SEC, temp_time[temp]);
				break;
			case 3:
				ds3231_Write(ADDRESS_DAY, temp_time[temp]);
				break;
			case 4:
				ds3231_Write(ADDRESS_DATE, temp_time[temp]);
				break;
			case 5:
				ds3231_Write(ADDRESS_MONTH, temp_time[temp]);
				break;
			case 6:
				ds3231_Write(ADDRESS_YEAR, temp_time[temp]);
				break;
		}
		temp++;
		if (temp > 6) {
			temp = 0;
		}
	}
}

void modifyAlarm() {
	count_alarm = (count_alarm + 1) % 10;
	if (count_alarm > 5) {
		switch (temp_alarm){
			case 0:
				lcd_ShowIntNum(70, 100, "  ", 2, BLACK, BLACK, 24);
				break;
			case 1:
				lcd_ShowIntNum(120, 100, "  ", 2, BLACK, BLACK, 24);
				break;
			case 2:
				lcd_ShowIntNum(170, 100, "  ", 2, BLACK, BLACK, 24);
				break;
		}
	} else {
		lcd_StrCenter(0, 50, Mode(), WHITE, BLACK, 24, 0);
		lcd_ShowIntNum(70, 100, alarm[0], 2, GREEN, BLACK, 24);
		lcd_ShowIntNum(120, 100, alarm[1], 2, GREEN, BLACK, 24);
		lcd_ShowIntNum(170, 100, alarm[2], 2, GREEN, BLACK, 24);
	}

	if (button_count[3] == 1) {
		alarm[temp_alarm]++;
		checkAlarm();
	}

	if (button_count[3] > 40 && button_count[3] % 4 == 1) {
		alarm[temp_alarm]++;
		checkAlarm();
	}

	if (button_count[12] == 1) {
		switch (temp_alarm){
			case 0:
				alarmPresent[temp_alarm] = alarm[temp_alarm];
				break;
			case 1:
				alarmPresent[temp_alarm] = alarm[temp_alarm];
				break;
			case 2:
				alarmPresent[temp_alarm] = alarm[temp_alarm];
				break;
		}
		temp_alarm++;
		if (temp_alarm > 2) {
			temp_alarm = 0;
		}
	}
}

void fsm() {
	switch (currentMode) {
		case modeInit:
			currentMode = modeNormal;
			break;
		case modeNormal:
			temp = 0;
			temp_alarm = 0;
			for (int i = 0; i < 7; i++) {
				temp_time[i] = 0;
				alarm[i] = 0;
			}
			displayTime();

			if (ds3231_hours == alarmPresent[0] && ds3231_min == alarmPresent[1]) {
				lcd_Fill(0, 200, 240, 300, RED);
				lcd_StrCenter(0, 240, "ALARM!", YELLOW, RED, 16, 0);
			}
			else {
				lcd_Fill(0, 200, 240, 300, BLACK);
				lcd_StrCenter(0, 240, "ALARM!", BLACK, BLACK, 16, 0);
			}

			if (button_count[0] == 1) {
				lcd_Fill(0, 50, 240, 75, BLACK);
				currentMode = modeModify;
			}
			break;
		case modeModify:
			for (int i = 0; i < 7; i++) {
				if (temp_time[i] == 0 && flag == 0) {
					switch (i) {
						case 0:
							temp_time[i] = ds3231_hours;
							break;
						case 1:
							temp_time[i] = ds3231_min;
							break;
						case 2:
							temp_time[i] = ds3231_sec;
							break;
						case 3:
							temp_time[i] = ds3231_day;
							break;
						case 4:
							temp_time[i] = ds3231_date;
							break;
						case 5:
							temp_time[i] = ds3231_month;
							break;
						case 6:
							temp_time[i] = ds3231_year;
							break;
					}
				}
			}
			flag = 1;
			modifyTime();
			if (button_count[0] == 1) {
				flag = 0;
				lcd_Fill(0, 50, 240, 75, BLACK);
				currentMode = modeAlarm;
			}
			break;
		case modeAlarm:
			lcd_ShowIntNum(20, 130, alarm[3], 2, BLACK, BLACK, 24);
			lcd_ShowIntNum(70, 130, alarm[4], 2, BLACK, BLACK, 24);
			lcd_ShowIntNum(120, 130, alarm[5], 2, BLACK, BLACK, 24);
			lcd_ShowIntNum(170, 130, alarm[6], 2, BLACK, BLACK, 24);
			for (int i = 0; i < 3; i++) {
				if (alarm[i] == 0 && flag == 0) {
					switch (i) {
						case 0:
							alarm[i] = alarmPresent[i];
							break;
						case 1:
							alarm[i] = alarmPresent[i];
							break;
						case 2:
							alarm[i] = alarmPresent[i];
							break;
					}
				}
			}
			flag = 1;
			modifyAlarm();
			if (button_count[0] == 1) {
				flag = 0;
				lcd_Fill(0, 50, 240, 75, BLACK);
				currentMode = modeNormal;
			}
			break;
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
