#ifndef _UART_H_
#define _UART_H_

#include "main.h"

#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "FIFO.h"


#define UART1 1

#define UART_REC_LEN  		200  	//�����������ֽ��� 200
#define EN_USART1_RX 		1		//ʹ�ܣ�1��/��ֹ��0������1����

extern UART_HandleTypeDef huart1;


#define RXBUFFERSIZE   1 					//�����С
extern 	uint8_t aRxBuffer[RXBUFFERSIZE];	//HAL��UART����Buffer


void UART_Init(uint8_t UART_ID);
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
int Write_Debug_UART(char* fmt,...);
	
#endif
