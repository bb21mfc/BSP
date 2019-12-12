#include "RTC.h"

RTC_HandleTypeDef RTC_Handler;


//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//返回值:SUCEE(1),成功
//      ERROR(0),进入初始化模式失败 
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

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
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
//设置时间戳信息
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
//获取时间戳信息
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

//RTC初始化
//返回值:0,初始化成功;
//       2,进入初始化模式失败;
uint8_t RTC_Init(void)
{      

	
	RTC_Handler.Instance			= RTC;
    RTC_Handler.Init.HourFormat		= RTC_HOURFORMAT_24;	//RTC设置为24小时格式 
    RTC_Handler.Init.AsynchPrediv	= 0X7F;           		//RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv	= 0XFF;            		//RTC同步分频系数(0~7FFF)   
    RTC_Handler.Init.OutPut			= RTC_OUTPUT_DISABLE;     
    RTC_Handler.Init.OutPutPolarity	= RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType		= RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK) 
		return 2;
      
//    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)//是否第一次配置
//    { 
        RTC_Set_Time(12,00,00,RTC_HOURFORMAT12_PM);	        //设置时间 ,根据实际时间修改
		RTC_Set_Date(19,12,3,2);		                    //设置日期
//        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//标记已经初始化过了
//    }
    return 0;
}

//RTC底层驱动，时钟配置
//此函数会被HAL_RTC_Init()调用
//hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE配置
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;                  //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
	__HAL_RCC_RTC_DISABLE();
	HAL_PWR_DisableBkUpAccess();
	__HAL_RCC_PWR_CLK_DISABLE();
	
}

//周期性唤醒定时器设置  
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(uint16_t cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,cnt-1,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);            //设置重装载值和时钟 
	
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void RTC_WKUP_IRQHandler(void)
{
	
  HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);

}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);

  //HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);   如果只唤醒一次则打开这个函数
	
}

