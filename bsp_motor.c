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
	//转速
extern float Shaft_Speed_A;
extern float Shaft_Speed_B;
extern int32_t Encoder_Overflow_Count_A;
extern int32_t Encoder_Overflow_Count_B;

extern u8  GetData;
extern float pitch,roll,yaw; 		//欧拉角
extern short aacx,aacy,aacz;		//加速度传感器原始数据
extern short gyrox,gyroy,gyroz;	//陀螺仪原始数据
extern short temp;					//暂存

extern float target1_speed;
extern float target2_speed;
extern float now_speed;
extern float output_speed;

//设置占空比函数，
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
A:前右轮
B:前左轮
C：后左轮
D：后右轮
*/
void CarGo(void)
{
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
		if(GetData)//GetData=0时 用于USMART调试MPU6050寄存器
		{
		//roll值（得出的就是°单位的角度）
			OLED_ShowString(24, 10, (uint8_t *)"Lambor",24);//注意前面换一下格式，不然有警告
			OLED_ShowString(24, 40, (uint8_t *)"Roll:",16);
			OLED_ShowNum(60, 40, (uint32_t)roll, 2, 24);
			OLED_Refresh_Gram();			//记得更新显示到OLED 
		//根据角度的不同设置不同的转速
		//为缓解上坡跑偏的问题，上坡时前轮速度比后轮略大
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
				printf("out of range！/r/n");
			}
		}
	}
	//用pid：
	//now_speed = motor_getSpeed();	//读取编码器测出的转速
	//output_speed = pidi_realize(target_speed, now_speed);
	
	//不用pid：
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
				printf("out of range！/r/n");
			}
		}
	}
	//用pid：
	//now_speed = motor_getSpeed();
	//output_speed = pidi_realize(target_speed, now_speed);
	
	//不用pid：
	output_speed = target1_speed;
	TIM_SetTIM2PWMA(1, output_speed);
	TIM_SetTIM2PWMB(1, output_speed);
  TIM_SetTIM2PWMC(1, output_speed);
	TIM_SetTIM2PWMD(1, output_speed);
}
/*
此处为了解决上坡时的驱动问题：
在平地上的转向和不同坡度的转向也是不同的
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

//获取转速
float motor_getSpeed(void)
{
	/* 当前时刻总计数值 = 计数器值 + 计数溢出次数 * 计数器溢出值 */
	Capture_Count_A =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle1) + (Encoder_Overflow_Count_A * (1000-1)); 
	Capture_Count_B =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle2) + (Encoder_Overflow_Count_B * (1000-1));
		
	/*M法 :转轴转速 = 单位时间内的计数值(编码器产生的脉冲数) / 编码器总分辨率 * 时间系数(将单位换算成秒) */
	Shaft_Speed_A = (float)(Capture_Count_A - Last_Count_A) / ENCODER_TOTAL_RESOLUTION * 10 ;
	Shaft_Speed_B = (float)(Capture_Count_B - Last_Count_B) / ENCODER_TOTAL_RESOLUTION * 10 ;

	/*输出轴转速 = 转轴转速 / 减速比 */
		
	//printf("speed：%.2f r/s \r\n", Shaft_Speed_A/REDUCTION_RATIO);
	//printf("B_speed：%.2f r/s \r\n", Shaft_Speed_B/REDUCTION_RATIO);
		
	/* 记录当前总计数值，供下一时刻计算使用 */
	Last_Count_B = Capture_Count_B;
	Last_Count_A = Capture_Count_A;
	return Shaft_Speed_A/REDUCTION_RATIO;
}


