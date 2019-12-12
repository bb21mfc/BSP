#include "IWDG.h"


IWDG_HandleTypeDef IWDG_Handler; //�������Ź����

//��ʼ���������Ź�
//prer:��Ƶ��:IWDG_PRESCALER_4~IWDG_PRESCALER_256
//rlr:�Զ���װ��ֵ,0~0XFFF.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).
void IWDG_Init(void)
{
    IWDG_Handler.Instance		= IWDG;
    IWDG_Handler.Init.Prescaler	= IWDG_PRESCALER_64;	//����IWDG��Ƶϵ��
    IWDG_Handler.Init.Reload	= 500;					//��װ��ֵ
	
    HAL_IWDG_Init(&IWDG_Handler);		//��ʼ��IWDG  
	
    __HAL_IWDG_START(&IWDG_Handler);	//�����������Ź�
}
    
//ι�������Ź�
void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&IWDG_Handler); 	//ι��
}

