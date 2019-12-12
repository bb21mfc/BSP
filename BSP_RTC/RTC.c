#include "RTC.h"

RTC_HandleTypeDef RTC_Handler;


//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//����ֵ:SUCEE(1),�ɹ�
//      ERROR(0),�����ʼ��ģʽʧ�� 
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours		 	 = hour;
	RTC_TimeStructure.Minutes	 	 = min;
	RTC_TimeStructure.Seconds	 	 = sec;
	RTC_TimeStructure.TimeFormat 	 = ampm;
	RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
	
	return HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}

//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Date		= date;
	RTC_DateStructure.Month		= month;
	RTC_DateStructure.WeekDay	= week;
	RTC_DateStructure.Year		= year;
	return HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);
}

struct tm Time_ConvUnixToCalender(time_t t)
{
	struct tm *t_tm;
	
	t_tm = localtime(&t);
	
	return *t_tm;
}
//����ʱ�����Ϣ
void Time_SetUnixTime(time_t t)
{
	struct tm t_tm;
	RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
	
	t_tm = Time_ConvUnixToCalender(t);
	
	RTC_DateStruct.Year  	= t_tm.tm_yday + 1900 - 2000;
	RTC_DateStruct.Month 	= t_tm.tm_mon + 1;
	RTC_DateStruct.Date	 	= t_tm.tm_mday;
	RTC_DateStruct.WeekDay	= t_tm.tm_wday;
	
	RTC_TimeStruct.Hours		  = t_tm.tm_hour;
	RTC_TimeStruct.Minutes		  = t_tm.tm_min;
	RTC_TimeStruct.Seconds		  = t_tm.tm_sec;
	RTC_TimeStruct.TimeFormat 	  = 0;
	RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;
	
	HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
}

time_t Time_ConvCalenderToUnix(struct tm t)
{
	return mktime(&t);
}
//��ȡʱ�����Ϣ
time_t Time_GetUnixTime(void)
{
	struct tm t_tm;
	time_t tmp;
	RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
	
	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	
	t_tm.tm_year 	= RTC_DateStruct.Year + 2000 - 1900;
	t_tm.tm_mon		= RTC_DateStruct.Month - 1;
	t_tm.tm_mday	= RTC_DateStruct.Date;
	t_tm.tm_wday	= RTC_DateStruct.WeekDay;
	
	t_tm.tm_hour	= RTC_TimeStruct.Hours;
	t_tm.tm_min		= RTC_TimeStruct.Minutes;
	t_tm.tm_sec		= RTC_TimeStruct.Seconds;
	
	tmp = Time_ConvCalenderToUnix(t_tm);
	
	return tmp;
}

void Dispaly_SyetemTime_RTC(void)
{
	struct tm t_tm;
	
	t_tm = Time_ConvUnixToCalender(Time_GetUnixTime());
	
	Write_Uart_DEBUG("Date:%02d-%02d-%02d    Time:%02d:%02d:%02d\r\n",
				t_tm.tm_year + 1900,t_tm.tm_mon + 1,t_tm.tm_mday,t_tm.tm_hour,t_tm.tm_min,t_tm.tm_sec);
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       2,�����ʼ��ģʽʧ��;
uint8_t RTC_Init(void)
{      

	
	RTC_Handler.Instance			= RTC;
    RTC_Handler.Init.HourFormat		= RTC_HOURFORMAT_24;	//RTC����Ϊ24Сʱ��ʽ 
    RTC_Handler.Init.AsynchPrediv	= 0X7F;           		//RTC�첽��Ƶϵ��(1~0X7F)
    RTC_Handler.Init.SynchPrediv	= 0XFF;            		//RTCͬ����Ƶϵ��(0~7FFF)   
    RTC_Handler.Init.OutPut			= RTC_OUTPUT_DISABLE;     
    RTC_Handler.Init.OutPutPolarity	= RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType		= RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK) 
		return 2;
      
//    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)//�Ƿ��һ������
//    { 
        RTC_Set_Time(12,00,00,RTC_HOURFORMAT12_PM);	        //����ʱ�� ,����ʵ��ʱ���޸�
		RTC_Set_Date(19,12,3,2);		                    //��������
//        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//����Ѿ���ʼ������
//    }
    return 0;
}

//RTC�ײ�������ʱ������
//�˺����ᱻHAL_RTC_Init()����
//hrtc:RTC���
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//ʹ�ܵ�Դʱ��PWR
	HAL_PWR_EnableBkUpAccess();//ȡ����������д����
    
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE����
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;                  //RTCʹ��LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//����ΪRTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTCʱ��ԴΪLSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTCʱ��ʹ��
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
	__HAL_RCC_RTC_DISABLE();
	HAL_PWR_DisableBkUpAccess();
	__HAL_RCC_PWR_CLK_DISABLE();
	
}

//�����Ի��Ѷ�ʱ������  
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(uint16_t cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//���RTC WAKE UP�ı�־
	
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,cnt-1,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);            //������װ��ֵ��ʱ�� 
	
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02); //��ռ���ȼ�1,�����ȼ�2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void RTC_WKUP_IRQHandler(void)
{
	
  HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);

}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);

  //HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);   ���ֻ����һ������������
	
}

