/*
 * uart.c
 *
 *  Created on: Nov 27, 2024
 *      Author: than
 */


#include "uart.h"
#include "global.h"

uint8_t receive_buffer1 = 0;
uint8_t receive_buffer2 = 0;
uint8_t msg[100];
uint8_t msg2[100];



void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString("0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString("-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%d",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString("0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString(".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}



void uart_init_uart2(){
	HAL_UART_Receive_IT(&huart2, &receive_buffer2, 1);
	HAL_GPIO_WritePin(ESP12_PWR_GPIO_Port, ESP12_PWR_Pin, 1);

}

void uart2_SendString(uint8_t* str){
	HAL_UART_Transmit(&huart2, (void*)msg2, sprintf((void*)msg2,"%s",str), 10);
}
void uart2_SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart2, bytes, size, 10);
}

