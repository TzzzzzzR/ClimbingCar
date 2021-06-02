/*串口使用说明：
huart1： 
		PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		中断优先级：0，0
		波特率：9600
重定义了fpuc：打印到蓝牙
接收中断函数：接收蓝牙发送来的信息
*/
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stdio.h"
#include "string.h"

//定义接收的相关变量
uint8_t ble_flag = 0;						//接收flag
char RxBuffer[RXBUFFERSIZE];		//接收数据	
uint8_t aRxBuffer;							//接收缓冲
uint8_t Uart1_Rx_Cnt = 0;				//接收缓冲计数

UART_HandleTypeDef huart1;


/* USART1 init function */

void MX_USART1_UART_Init(void)
{


  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
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
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);

	}
}
/* USER CODE BEGIN 1 */

	  
#if 1
#pragma import(__use_no_semihosting)
//这条语句可以关闭半主机模式，只需要在任意一个C文件中加入即可。
//至于下面的_sys_exit()是有可能被要求重写的函数
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 

int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&ble_uart, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

#endif

//接收中断函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

	if(huart->Instance == USART1)
	{
		ble_flag = 1;
		Uart1_Rx_Cnt = 0;												//计数清零
		memset(RxBuffer,0x00,sizeof(RxBuffer)); // 清空存放数据的数组
		if(Uart1_Rx_Cnt >= 255)									//判断是否溢出
		{
			HAL_UART_Transmit(&huart1,(uint8_t *)"outof range",12,0xffff);
		}
		else
		{
			RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer;//接收数据转存		
			if((RxBuffer[Uart1_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart1_Rx_Cnt-2] == 0x0D))
			{
					HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF); //此处把接收到的数据发送出去
						while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
			}
		}
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);//再次开启中断
	}
}


	


