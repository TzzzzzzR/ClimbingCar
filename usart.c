/*����ʹ��˵����
huart1�� 
		PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		�ж����ȼ���0��0
		�����ʣ�9600
�ض�����fpuc����ӡ������
�����жϺ�����������������������Ϣ
*/
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stdio.h"
#include "string.h"

//������յ���ر���
uint8_t ble_flag = 0;						//����flag
char RxBuffer[RXBUFFERSIZE];		//��������	
uint8_t aRxBuffer;							//���ջ���
uint8_t Uart1_Rx_Cnt = 0;				//���ջ������

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
//���������Թرհ�����ģʽ��ֻ��Ҫ������һ��C�ļ��м��뼴�ɡ�
//���������_sys_exit()���п��ܱ�Ҫ����д�ĺ���
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 

int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&ble_uart, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

#endif

//�����жϺ���
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

	if(huart->Instance == USART1)
	{
		ble_flag = 1;
		Uart1_Rx_Cnt = 0;												//��������
		memset(RxBuffer,0x00,sizeof(RxBuffer)); // ��մ�����ݵ�����
		if(Uart1_Rx_Cnt >= 255)									//�ж��Ƿ����
		{
			HAL_UART_Transmit(&huart1,(uint8_t *)"outof range",12,0xffff);
		}
		else
		{
			RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer;//��������ת��		
			if((RxBuffer[Uart1_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart1_Rx_Cnt-2] == 0x0D))
			{
					HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF); //�˴��ѽ��յ������ݷ��ͳ�ȥ
						while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
			}
		}
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);//�ٴο����ж�
	}
}


	


