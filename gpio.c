/*
GPIO使用说明：
控制四个电机:（具体宏定义见bsp_motor.h）
PB0 PB1， PB14 PB15， PB12 PB13， PB8 PB9控制四个电机

循迹：
PB3 PB4 PB5

oled:
PA15 PA8 PA4 PA5 PC13(除了RST以外的IO口初始化直接在oled_Init实现了)

mpu6050:
PB10 PB11

*/
#include "gpio.h"
#include "bsp_motor.h"
#include "bsp_trail.h"
#include "bsp_oled.h"

void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, AIN1|AIN2|BIN1|BIN2|CIN1|CIN2|DIN1|DIN2, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOA, OLED_RST_GPIO_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin =  OLED_RST_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
  /*Configure GPIO pins : PB0 PB1 PB14 PB15 PB12 PB13 PB8 PB9 */
  GPIO_InitStruct.Pin = AIN1|AIN2|BIN1|BIN2|CIN1|CIN2|DIN1|DIN2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = XUNJI_1_Pin|XUNJI_3_Pin|XUNJI_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

