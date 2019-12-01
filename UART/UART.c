#include "UART.h"


extern app_fifo_t Uart_Fifo;

UART_HandleTypeDef huart1;

uint8_t aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void UART_Init(uint8_t UART_ID)
{
	switch(UART_ID)
	{
		case 1:
			
			huart1.Instance 			= USART1;
			huart1.Init.BaudRate 		= 115200;
			huart1.Init.WordLength 		= UART_WORDLENGTH_8B;
			huart1.Init.StopBits 		= UART_STOPBITS_1;
			huart1.Init.Parity 			= UART_PARITY_NONE;
			huart1.Init.Mode 			= UART_MODE_TX_RX;
			huart1.Init.HwFlowCtl 		= UART_HWCONTROL_NONE;
			huart1.Init.OverSampling 	= UART_OVERSAMPLING_16;
		
			if (HAL_UART_Init(&huart1) != HAL_OK)
			{
				Error_Handler();
			}
			if(HAL_UART_Receive_IT(&huart1,aRxBuffer,RXBUFFERSIZE))
			{
				
			}
			break;
					
		default:
			
			break;
		}

		
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
  if(huart->Instance==USART1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
#if EN_USART1_RX
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif	

  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
		
  }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	//Write_Debug_UART("interrupt111 fifo:%d\r\n",fifo_length(&Uart_Fifo));
	app_fifo_write(&Uart_Fifo,aRxBuffer,RXBUFFERSIZE);
	//Write_Debug_UART("interrupt fifo:%d\r\n",fifo_length(&Uart_Fifo));
	
	HAL_UART_Receive_IT(&huart1,aRxBuffer,RXBUFFERSIZE);
}


int Write_Debug_UART(char* fmt,...)
{
	va_list ap;
	char buffer[256];
	uint16_t i = 0;
	uint16_t lastprintf = 0;
	
	memset(buffer,0,sizeof(buffer));
	
	va_start(ap,fmt);
	lastprintf = vsnprintf(buffer,sizeof(buffer),fmt,ap);
	va_end(ap);
	
	while(i < lastprintf)
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)&buffer[i],1,1000);
		i++;
	}

	return i;
}
