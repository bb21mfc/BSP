#ifndef _RTC_H_
#define _RTC_H_

#include "main.h"
#include "UART.h"
#include "time.h"

extern RTC_HandleTypeDef RTC_Handler;  //RTC���
    
uint8_t RTC_Init(void);              //RTC��ʼ��
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);	//RTC��������
void RTC_Set_WakeUp(uint16_t cnt);

void Time_SetUnixTime(time_t t);
time_t Time_GetUnixTime(void);
void Dispaly_SyetemTime_RTC(void);

#endif

