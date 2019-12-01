#ifndef _UART_H_
#define _UART_H_

#include "main.h"

#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "FIFO.h"


#define UART1 1

#define UART_REC_LEN  		200  	//定义最大接收字节数 200
#define EN_USART1_RX 		1		//使能（1）/禁止（0）串口1接收

extern UART_HandleTypeDef huart1;


#define RXBUFFERSIZE   1 					//缓存大小
extern 	uint8_t aRxBuffer[RXBUFFERSIZE];	//HAL库UART接收Buffer


void UART_Init(uint8_t UART_ID);
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
int Write_Debug_UART(char* fmt,...);
	
#endif
