/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"
#include "time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INIT 0
#define DRAW 1
#define CLEAR 2
#define SNAKE_MAX_SIZE	100

#define X0	0
#define X1	4
#define X2	235
#define X3	239

#define Y0	49
#define Y1	53
#define Y2	216
#define Y3	219

#define XBUTTON 71
#define YBUTTON Y3 + 3
#define RADIUS	3

struct Point{
	uint16_t x, y, color;
};

enum Direction{
	up,
	down,
	left,
	right
};

struct Snake{
	struct Point node[SNAKE_MAX_SIZE];
	uint8_t len;
	enum Direction direction;
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int draw_Status = INIT;

uint8_t clearNodeIdx = 0;
struct Snake snake;
struct Point prey;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void touchProcess();


uint8_t isStartPress();
uint8_t isUpButtonPress();
uint8_t isDownButtonPress();
uint8_t isLeftButtonPress();
uint8_t isRightButtonPress();

double distance(uint16_t, uint16_t, uint16_t, uint16_t);
void randomPrey();
void displayFixedItems();
void Init();
void display();
void control();
uint8_t handle();
void playGame();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_FSMC_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  system_init();
//  touch_Adjust();
  lcd_Clear(BLACK);

  uint8_t option = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(option == 0){
		  // draw start button
		  lcd_Clear(WHITE);
		  lcd_Fill(60, 140, 180, 180, GREEN);
		  lcd_ShowStr(90, 145, "START", WHITE, GREEN, 24, 1);
		  option = 1;
	  }
	  else if(option == 1){
		  // play game or not
		  // read screen
		  //&& isStartPress()
		  touch_Scan();
		  if(touch_IsTouched() && isStartPress()){
			  // clear start button
			  lcd_Clear(WHITE);
			  playGame();
			  // after lose game
			  option = 0;
		  }
	  }


	  HAL_Delay(50);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	timer_init();
	button_init();
	lcd_init();
	touch_init();
	setTimer2(10);
}

uint8_t count_led_debug = 0;
void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

uint8_t isButtonClear(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 &&
			touch_GetY() > 10 && touch_GetY() < 60;
}

void touchProcess(){
	switch (draw_Status) {
		case INIT:
			lcd_Fill(60, 10, 180, 60, GBLUE);
			lcd_ShowStr(90, 20, "CLEAR", RED, BLACK, 24, 1);
			draw_Status = DRAW;
			break;
		case DRAW:
			if(isButtonClear()){
				draw_Status = CLEAR;
				lcd_Fill(0, 60, 240, 320, BLACK);
				lcd_Fill(60, 10, 180, 60, GREEN);
				lcd_ShowStr(90, 20, "CLEAR", RED, BLACK, 24, 1);
			}
			break;
		case CLEAR:
			if(!touch_IsTouched()) draw_Status = INIT;
			break;
		default:
			break;
	}
}





uint8_t isStartPress(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 &&
			touch_GetY() > 140 && touch_GetY() < 180;
}

uint8_t isUpButtonPress(){
	return touch_GetX() >= 102 && touch_GetX() <= 142 &&
			touch_GetY() >= 222 && touch_GetY() <= 262;
}

uint8_t isDownButtonPress(){
	// 102 <= x <= 129
	// 270 <= y <= 297
	return touch_GetX() >= 102 && touch_GetX() <= 142 &&
			touch_GetY() >= 272 && touch_GetY() <= 312;
}

uint8_t isLeftButtonPress(){
	// 74 <= x <= 101
	// 270 <= y <= 297
	return touch_GetX() >= 52 && touch_GetX() <= 92 &&
			touch_GetY() >= 272 && touch_GetY() <= 312;
}

uint8_t isRightButtonPress(){
	// 130 <= x <= 157
	// 270 <= y <= 297
	return touch_GetX() >= 152 && touch_GetX() <= 192 &&
			touch_GetY() >= 272 && touch_GetY() <= 312;
}

double distance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
		return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

void randomPrey(){
	prey.x = rand() % ((X2 - 5) - (X1 + 5) + 1) + (X1 + 5);
	prey.y = rand() % ((Y2 - 5) - (Y1 + 5) + 1) + (Y1 + 5);
	prey.color = RED;
}

void displayFixedItems(){
	// hien thi khung
	// khung tren
	lcd_Fill(X0, Y0, X3, Y1, BLUE);
	// khung duoi
	lcd_Fill(X0, Y2, X3, Y3, BLUE);
	// khung trai
	lcd_Fill(X0, Y0, X1, Y3, BLUE);
	// khung phai
	lcd_Fill(X2, Y0, X3, Y3, BLUE);

	//hien thi thanh dieu khien 4 nut
	// up
	lcd_Fill(102, 222, 142, 262, GREEN);
	// down
	lcd_Fill(102, 272, 142, 312, GREEN);
	// left
	lcd_Fill(52, 272, 92, 312, GREEN);
	// right
	lcd_Fill(152, 272, 192, 312, GREEN);

	// hien thi score
	lcd_StrCenter(10, 10, "Score: ", BLUE, WHITE, 16, 1);
	lcd_ShowIntNum(160, 10, 1, 3, BLUE, WHITE, 16);
}

void Init(){
	// khoi tao con ran
	snake.len = 2;
	snake.direction = right;
	clearNodeIdx = 1;

	// head node
	snake.node[0].x = X1 + 3*RADIUS + 2;
	snake.node[0].y = Y1 + RADIUS + 1;

	snake.node[0].color = GRAY;

	// clear node
	snake.node[clearNodeIdx].x = X1 + RADIUS + 1;
	snake.node[clearNodeIdx].y = Y1 + RADIUS + 1;
	snake.node[clearNodeIdx].color = WHITE;

	// khoi tao con moi
	randomPrey();

}

void display(){
	// ve con ran
	for(uint8_t i = 0; i < snake.len; i++){
		lcd_DrawCircle(snake.node[i].x, snake.node[i].y, snake.node[i].color, RADIUS, 1);
	}

	// ve con moi
	lcd_DrawCircle(prey.x, prey.y, prey.color, RADIUS, 1);
}

void control(){
	// nhan 4 nut dieu huong

	// doc man hinh
	touch_Scan();

	if(touch_IsTouched()){
		if(isUpButtonPress()){
			snake.direction = up;
		}
		else if(isDownButtonPress()){
			snake.direction = down;
		}
		else if(isLeftButtonPress()){
			snake.direction = left;
		}
		else if(isRightButtonPress()){
			snake.direction = right;
		}
	}

}

uint8_t handle(){
	// cap nhat vi tri than con ran
	for(uint8_t i = snake.len - 1; i > 0; i--){
		snake.node[i].x = snake.node[i - 1].x;
		snake.node[i].y = snake.node[i - 1].y;
	}
	// cap nhat vi tri dau con ran
	switch(snake.direction){
		case up:
			snake.node[0].y -= 2*RADIUS + 1;
			break;
		case down:
			snake.node[0].y += 2*RADIUS + 1;
			break;
		case left:
			snake.node[0].x -= 2*RADIUS + 1;
			break;
		case right:
			snake.node[0].x += 2*RADIUS + 1;
			break;
		default:
			break;
	}

	// xu ly cham bien
	if(snake.node[0].x <= X1 || snake.node[0].x >= X2
		|| snake.node[0].y <= Y1 || snake.node[0].y >= Y2){
		return 1;
	}

	// xu ly an duoi chinh minh : khong check clear node
	for(uint8_t i = 1; i < snake.len - 1; i++){
		if(snake.node[i].x == snake.node[0].x && snake.node[i].y == snake.node[0].y){
			return 1;
		}
	}

	// xu ly an con moi
	if(distance(snake.node[0].x, snake.node[0].y, prey.x, prey.y) < 1.5*(double)RADIUS){
		snake.len++;
		// xoa con moi
		lcd_DrawCircle(prey.x, prey.y, WHITE, RADIUS, 1);
		// bien clear node thanh node thuong
		snake.node[clearNodeIdx].color = GRAY;

		// tao clear node moi
		snake.node[snake.len - 1] = snake.node[clearNodeIdx];
		snake.node[snake.len - 1].color = WHITE;

		switch (snake.direction) {
			case up:
				snake.node[snake.len - 1].y += 2*RADIUS + 1;
				break;
			case down:
				snake.node[snake.len - 1].y -= 2*RADIUS + 1;
				break;
			case left:
				snake.node[snake.len - 1].x += 2*RADIUS + 1;
				break;
			case right:
				snake.node[snake.len - 1].x -= 2*RADIUS + 1;
				break;
			default:
				break;
		}
		clearNodeIdx = snake.len - 1;
		// cap nhat score
		lcd_ShowIntNum(160, 10, snake.len - 1, 3, BLUE, WHITE, 16);
		// tao con moi moi
		randomPrey();
	}
	return 0;
}

uint8_t counter = 50;

void playGame(){
	// ve khung + thanh dieu khien + score
	displayFixedItems();

	// khoi tao con ran va con moi
	Init();
	while(1){
		if(flag_timer2){
			setTimer2(10);

			if(counter <= 0){
				// ve ran + con moi
//				counter = 10;
				display();
			}

			// dieu khien
			control();

			if(counter <= 0){
				counter = 50;
				// xu ly
				if(handle()){
					break;
				}
			}
			counter--;
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
