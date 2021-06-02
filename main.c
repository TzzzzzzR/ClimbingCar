/*
工程说明：（主要功能以及相应文件）
1.差速转向：
（1）关于电机：
基本电平：gpio.c 
电机驱动与转向函数：bsp_motor.c
PWM输出：tim.c

2.循迹：
（2）关于红外传感器：
输入引脚：gpio.c
循迹函数：bsp_trail.c

3.pid闭环控制电机：
（1）读取编码器：tim.c
（2）转速的计算：stm32f1xx_it.c main.c
（3）pid的实现：bsp_pid.c

3.蓝牙通讯：
（1）蓝牙通讯：usart.c

4.mpu6050读取角度：（参考正点原子）
（1）IIC通信：mpuiic.c
（2）初始化以及接口函数：mpu6050.c

5.oled显示：
（1）显示函数：bsp_oled.c
（2）模拟引脚初始化：gpio.c
*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "bsp_motor.h"
#include "bsp_trail.h"
#include "bsp_pid.h"
#include "bsp_oled.h"
#include "stdio.h"
#include "delay.h"
#include "sys.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

void SystemClock_Config(void);

	//串口收发
extern uint8_t aRxBuffer;
extern char RxBuffer[RXBUFFERSIZE];
extern uint8_t ble_flag;

	//记录encoder_tim的溢出次数，用来计算转速
int32_t Encoder_Overflow_Count_A = 0;
int32_t Encoder_Overflow_Count_B = 0;

	//获得当前电机的转向 0为正、1为负
int Motor_Direction_A  = 0;
int Motor_Direction_B  = 0;
	//获得当前计数值
int32_t Capture_Count_A = 0;
int32_t Capture_Count_B = 0;
	//上一次计数值
int32_t Last_Count_A = 0;
int32_t Last_Count_B = 0;
	//转速
float Shaft_Speed_A = 0.0f;
float Shaft_Speed_B = 0.0f;

	//电机速度控制相关变量
float target1_speed = 0;
float target2_speed = 0;
float now_speed = 0;
float output_speed = 0;

	//陀螺仪相关变量（本工程中只需roll即可）
u8 GetData=1;
float pitch,roll,yaw; 		//欧拉角
short aacx,aacy,aacz;			//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
short temp;								//暂存

int main(void)
{
	HAL_Init();
  //SystemClock_Config();
  Stm32_Clock_Init(RCC_PLL_MUL9); //设置时钟,72M
	MX_GPIO_Init();
	MX_USART1_UART_Init();
  MX_TIM2_Init();
	MX_TIM3_Init();
  MX_TIM4_Init();
	//初始化oled
	OLED_Init(); 
	//使能更新中断定时器
	HAL_TIM_Base_Start_IT(&htim1);
	//使能串口接收中断
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&aRxBuffer,1);
	//使能pwm通道
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	//初始化延时函数
	delay_init(72);                
	//使能编码器
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	//MPU DMP初始化
	printf("MPU6050 TEST\r\n");
  while(mpu_dmp_init())
	{
	  printf("MPU6050 Error!!!\r\n");
		delay_ms(500);
	}
  printf("MPU6050 OK\r\n");
	//pid初始化
	float p = 0.2;
	float i = 0.8;
	float d = 0.01;
	pid_loc_init(p, i, d);
	pid_inc_init(p, i, d);
	while (1)
  {
		Read_XUNJI_Date(); //读循迹线值
		if(XUNJI_1_IN==1&&XUNJI_2_IN==1&&XUNJI_3_IN==1)//111
	  {
			CarGo();	
			HAL_Delay(5);
	  }
		 if((XUNJI_1_IN==0&&XUNJI_2_IN==1&&XUNJI_3_IN==0))//010
	  {
			CarGo();			
			HAL_Delay(5);
	  }
		if(XUNJI_1_IN==0&&XUNJI_2_IN==0&&XUNJI_3_IN==1)//001
	  {
			CarBigRight();
			HAL_Delay(5);
	  }
		if(XUNJI_1_IN==0&&XUNJI_2_IN==1&&XUNJI_3_IN==1)//011
	  {
			CarRight();
			HAL_Delay(5);
		}
		if(XUNJI_1_IN==1&&XUNJI_2_IN==1&&XUNJI_3_IN==0)//110
	  {
			CarLeft();
			HAL_Delay(5);
	  }
		if((XUNJI_1_IN==1&&XUNJI_2_IN==0&&XUNJI_3_IN==0))//100
	  {
			CarBigLeft();
			HAL_Delay(5);
	  }
		if(XUNJI_1_IN==0&&XUNJI_2_IN==0&&XUNJI_3_IN==0)//000 
	  {
			CarStop();
		}
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
