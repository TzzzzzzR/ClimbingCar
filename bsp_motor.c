#include "bsp_motor.h"
#include "stdio.h"
#include "bsp_pid.h"
#include "delay.h"
#include "sys.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "bsp_oled.h"

extern int32_t Capture_Count_A;
extern int32_t Capture_Count_B;
extern int32_t Last_Count_A;
extern int32_t Last_Count_B;
	//ת��
extern float Shaft_Speed_A;
extern float Shaft_Speed_B;
extern int32_t Encoder_Overflow_Count_A;
extern int32_t Encoder_Overflow_Count_B;

extern u8  GetData;
extern float pitch,roll,yaw; 		//ŷ����
extern short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
extern short gyrox,gyroy,gyroz;	//������ԭʼ����
extern short temp;					//�ݴ�

extern float target1_speed;
extern float target2_speed;
extern float now_speed;
extern float output_speed;

//����ռ�ձȺ�����
void TIM_SetTIM2PWMA(int direction, float compare)
{
	if(direction == 1){
		HAL_GPIO_WritePin(GPIOB, AIN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, AIN2, GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, AIN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, AIN2, GPIO_PIN_SET);
	}
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, compare);
} 

void TIM_SetTIM2PWMB(int direction, float compare)
{
	if(direction == 1){
		HAL_GPIO_WritePin(GPIOB, BIN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, BIN2, GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, BIN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, BIN2, GPIO_PIN_SET);
	}
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, compare);
} 

void TIM_SetTIM2PWMC(int direction, float compare)
{
	if(direction == 1){
		HAL_GPIO_WritePin(GPIOB, CIN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, CIN2, GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, CIN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, CIN2, GPIO_PIN_SET);
	}
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, compare);
} 

void TIM_SetTIM2PWMD(int direction, float compare)
{
	if(direction == 1){
		HAL_GPIO_WritePin(GPIOB, DIN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, DIN2, GPIO_PIN_RESET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, DIN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, DIN2, GPIO_PIN_SET);
	}
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, compare);
} 
/*
A:ǰ����
B:ǰ����
C��������
D��������
*/
void CarGo(void)
{
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		if(GetData)//GetData=0ʱ ����USMART����MPU6050�Ĵ���
		{
		//rollֵ���ó��ľ��ǡ㵥λ�ĽǶȣ�
			OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);//ע��ǰ�滻һ�¸�ʽ����Ȼ�о���
			OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
			OLED_ShowNum(60, 40, (uint32_t)roll, 2, 24);
			OLED_Refresh_Gram();			//�ǵø�����ʾ��OLED 
		//���ݽǶȵĲ�ͬ���ò�ͬ��ת��
		//Ϊ����������ƫ�����⣬����ʱǰ���ٶȱȺ����Դ�
			if(roll <= 12 )
			{
				target1_speed = 196;
				target2_speed = 196;
			}
			else if(roll>12&&roll<20)
				
			{
				target1_speed = 230;
				target2_speed = 230;
			}
			else if(roll>20&&roll<=32)
			{
				target1_speed = 360;
				target2_speed = 300;
			}
			else if(roll > 32 && roll<38)
			{
				target1_speed = 475;
				target2_speed = 450;
			}
			else if(roll>=38&&roll<47)
			{
				target1_speed = 549;
				target2_speed = 500;
			}
			else
			{
				printf("out of range��/r/n");
			}
		}
	}
	//��pid��
	//now_speed = motor_getSpeed();	//��ȡ�����������ת��
	//output_speed = pidi_realize(target_speed, now_speed);
	
	//����pid��
	TIM_SetTIM2PWMA(1, target1_speed);
	TIM_SetTIM2PWMB(1, target1_speed);
  TIM_SetTIM2PWMC(1, target2_speed);
	TIM_SetTIM2PWMD(1, target2_speed);
}

void CarStop(void)
{
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	
		if(GetData)
		{
			OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);
			OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
			OLED_ShowNum(60 , 40, (uint32_t)roll, 2, 24);
			OLED_Refresh_Gram();		

			if(roll <= 10 )
			{
				target1_speed = 10;
			}
			else if(roll>10&&roll<20)
			{
				target1_speed = 100;
			}
			else if(roll>20&&roll<35)
			{
				target1_speed = 180;
			}
			else if(roll > 32 && roll<38)
			{
				target1_speed = 250;
			}
			else if(roll>=38&&roll<47)
			{
				target1_speed = 300;
			}
			else
			{
				printf("out of range��/r/n");
			}
		}
	}
	//��pid��
	//now_speed = motor_getSpeed();
	//output_speed = pidi_realize(target_speed, now_speed);
	
	//����pid��
	output_speed = target1_speed;
	TIM_SetTIM2PWMA(1, output_speed);
	TIM_SetTIM2PWMB(1, output_speed);
  TIM_SetTIM2PWMC(1, output_speed);
	TIM_SetTIM2PWMD(1, output_speed);
}
/*
�˴�Ϊ�˽������ʱ���������⣺
��ƽ���ϵ�ת��Ͳ�ͬ�¶ȵ�ת��Ҳ�ǲ�ͬ��
*/
void CarLeft(void)
{
	OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);
	OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
	OLED_ShowNum(60 , 40, (uint32_t)roll, 2, 24);
	OLED_Refresh_Gram();	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	
		if(GetData)
		{
			if(roll < 7)
			{
				TIM_SetTIM2PWMA(1, 150);
				TIM_SetTIM2PWMB(1, 20);
				TIM_SetTIM2PWMC(1, 20);
				TIM_SetTIM2PWMD(1, 550);
			}
			else
			{
				CarGo();
			}
		}
	}
}

void CarRight(void)
{
	OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);
	OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
	OLED_ShowNum(60 , 40, (uint32_t)roll, 2, 24);
	OLED_Refresh_Gram();			
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	
		if(GetData)
		{
			if(roll < 7)
			{
				TIM_SetTIM2PWMA(1, 20);
				TIM_SetTIM2PWMB(1, 150);
				TIM_SetTIM2PWMC(1, 550);
				TIM_SetTIM2PWMD(1, 20);
			}
			else
			{
				CarGo();
			}
		}
	}
}

void CarBigRight(void)
{
	OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);
	OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
	OLED_ShowNum(60 , 40, (uint32_t)roll, 2, 24);
	OLED_Refresh_Gram();		
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	
		if(GetData)
		{
			if(roll<=20)
			{
				TIM_SetTIM2PWMA(0, 150);
				TIM_SetTIM2PWMB(1, 150);
				TIM_SetTIM2PWMC(1, 600);
				TIM_SetTIM2PWMD(1, 30);
			}
			else if(roll>20&&roll<=32)
			{
				CarClimb_Right();
			}else
			{
				CarClimb__Right();
			}
		}
	}
}

void CarBigLeft(void)
{
	OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);
	OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
	OLED_ShowNum(60 , 40, (uint32_t)roll, 2, 24);
	OLED_Refresh_Gram();		
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	
		if(GetData)
		{
			if(roll<=20)
			{
				TIM_SetTIM2PWMA(1, 150);
				TIM_SetTIM2PWMB(0, 150);
				TIM_SetTIM2PWMC(1, 30);
				TIM_SetTIM2PWMD(1, 600);
			}
			else if(roll>20&&roll<=32)
			{
				CarClimb_Left();
			}
			else
			{
				CarClimb__Left();
			}
		}
	}	
}

void CarClimb_Left(void)
{
	TIM_SetTIM2PWMA(1, 300);
	TIM_SetTIM2PWMB(0, 300);
	TIM_SetTIM2PWMC(1, 600);
	TIM_SetTIM2PWMD(1, 800);
}

void CarClimb_Right(void)
{
	TIM_SetTIM2PWMA(0, 300); 
	TIM_SetTIM2PWMB(1, 300);
	TIM_SetTIM2PWMC(1, 800);
	TIM_SetTIM2PWMD(1, 600);
}

void CarClimb__Left(void)
{
	TIM_SetTIM2PWMA(1, 400);
	TIM_SetTIM2PWMB(0 ,100);
	TIM_SetTIM2PWMC(1, 400);
	TIM_SetTIM2PWMD(1, 700);
}

void CarClimb__Right(void)
{
	TIM_SetTIM2PWMA(0, 100); 
	TIM_SetTIM2PWMB(1, 400);
	TIM_SetTIM2PWMC(1, 700);
	TIM_SetTIM2PWMD(1, 400);
}

//��ȡת��
float motor_getSpeed(void)
{
	/* ��ǰʱ���ܼ���ֵ = ������ֵ + ����������� * ���������ֵ */
	Capture_Count_A =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle1) + (Encoder_Overflow_Count_A * (1000-1)); 
	Capture_Count_B =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle2) + (Encoder_Overflow_Count_B * (1000-1));
		
	/*M�� :ת��ת�� = ��λʱ���ڵļ���ֵ(������������������) / �������ֱܷ��� * ʱ��ϵ��(����λ�������) */
	Shaft_Speed_A = (float)(Capture_Count_A - Last_Count_A) / ENCODER_TOTAL_RESOLUTION * 10 ;
	Shaft_Speed_B = (float)(Capture_Count_B - Last_Count_B) / ENCODER_TOTAL_RESOLUTION * 10 ;

	/*�����ת�� = ת��ת�� / ���ٱ� */
		
	//printf("speed��%.2f r/s \r\n", Shaft_Speed_A/REDUCTION_RATIO);
	//printf("B_speed��%.2f r/s \r\n", Shaft_Speed_B/REDUCTION_RATIO);
		
	/* ��¼��ǰ�ܼ���ֵ������һʱ�̼���ʹ�� */
	Last_Count_B = Capture_Count_B;
	Last_Count_A = Capture_Count_A;
	return Shaft_Speed_A/REDUCTION_RATIO;
}


