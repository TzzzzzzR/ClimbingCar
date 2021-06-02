#include <stdio.h>
#include <stdlib.h>
#include "bsp_oled.h"
#include "bsp_oledfont.h"

//OLED的显存
//存放格式如下.共8页
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
uint8_t OLED_GRAM[128][8];//一个二维数组，先操作这个数组，最后再一次性写入

#if OLED_MODE == 1
//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	DATAOUT(dat);	    
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		   

	OLED_WR_Clr();	 
	OLED_WR_Set();
  
	OLED_DC_Set();	 
} 	    	    
#else
//向SSD1106写入一个字节。

//此处为用IO口输出模拟SPI，故需要查看SPI时序图：
//dat:要写入的数据/命令
//由SSD1106手册：cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{	
	unsigned char i;
	OLED_CS_Clr();			//CS:片选信号线：下降开始上升结束
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  

	for(i=0;i<8;i++)		//循环发送8位（一字节）
	{			  
		OLED_SCLK_Clr();	//SCLK拉低：在 SCLK 的上升沿，数据从 SDIN 移入到SSD1306
		if(dat&0x80)			//传输数据（把dat的每一位都陆续赋给SDIN移入SSD1306）
		   OLED_SDIN_Set();	
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   				//高位在前，从高往后读
	}				 		  
	OLED_CS_Clr();			//片选信号线上升，信号传输结束
	OLED_DC_Set();   	  //默认置高
} 
#endif
//查看SSD1306手册（正点开发指南里也有提）可得，写入以下命令能达到效果

//发现直接用二维数组来操纵更方便(如下OLED_Refresh_Gram)
//设置起始地址的函数，
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//0XB0~B7，该命令用于设置页地址，其低三位的值对应着 GRAM 的页地址
	//无符号类型右移（视为逻辑右移），高位补0（有符号类型算数位移高位补1）
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//0X10~0X1F，该指令用于设置显示时的起始列地址高四位。
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); //0X00~0X0F，该指令用于设置显示时的起始列地址低四位
} 
	 

//更新显存到LCD：首地址设为0,0. 然后再通过二维数组写入数据		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//开启OLED显示 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令	开启电荷泵设置
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON				电荷泵开
	OLED_WR_Byte(0XAF,OLED_CMD);  //设置显示开关：	o AEh : Display OFF o AFh : Display ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的，和没点亮效果一样	  
/*void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}*/
//数组操作版：
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;						//64行（y）共8页（pos）
	bx=y%8;								//得到是那一页的第几位
	temp=1<<(7-bx);				//每一页8位，把1移到锁定的位（注意64排起来每8位是从高到低的，故7-bx）
	if(t)OLED_GRAM[x][pos]|=temp;//OLED_GRAM[x][pos]得到的是一个8位的数据，要操作到点
	else OLED_GRAM[x][pos]&=~temp;	    
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);	//得到字体一个字符对应点阵集所占的字节数
	
	chr=chr-' ';//得到偏移后的值	？————————————————————这句没看懂————————————————————？
  for(t=0;t<csize;t++)
  { 
		//asc2_1206为ASCII字符集点阵（二维数组）
		//传入的chr可以用asc2_1206[chr]表达出来，而t用来遍历这一行
		
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;															//没有的字库
		
		for(t1=0;t1<8;t1++)//每一个t对应一个八位数，用t1来遍历
		{
			//每一位根据0/1显示出来相反，这或许就是ASCII字符集点阵的原理
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)											
			{
				y=y0;
				x++;						//点完一列y就x++点下一列
				break;
			}
		}  	 
  }          
}


/*与上一个函数相同（区别：没有用OLED_DrawPoint；形参有一点区别）换着用的时候记得改
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}*/
//m^n函数
//一个简单的数学函数，显示数字要用
unsigned long oled_pow(unsigned char m,unsigned char n)
{
	unsigned long result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)											//循环一次写一位
	{
		temp=(num/oled_pow(10,len-t-1))%10;		//得到每一位的数字是多少
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)													//最高位为0时
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);//打出一个' '
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); //temp+'0'为传入的一个字符，通过OLED_ShowChar打出来
	}
} 

//以下是用第二种showchar的shounum
/*void OLED_ShowNum(unsigned char x,unsigned char y,unsigned long num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
}*/
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}//由起点坐标和size原因超过显示范围的处理
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
}	 

/*(2)同上
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}*/


//初始化SH1106					    
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	//CS
	GPIO_InitStruct.Pin = OLED_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_CS_GPIO, &GPIO_InitStruct);

	//DC
	GPIO_InitStruct.Pin = OLED_DC_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_DC_GPIO, &GPIO_InitStruct);

	//SCLK
	GPIO_InitStruct.Pin = OLED_SCLK_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_SCLK_GPIO, &GPIO_InitStruct);

	//SDIN
	GPIO_InitStruct.Pin = OLED_SDIN_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_SDIN_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_GPIO_PIN, GPIO_PIN_SET);
 	HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_SET);


  OLED_RST_Set();
	HAL_Delay(100);
	OLED_RST_Clr();
	HAL_Delay(100);
	OLED_RST_Set();

	//可查阅手册得：				  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel（关闭显示）
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address（[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;）
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)（设置显示开始行 [5:0],行数）
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register（对比度设置）
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness（1~255;默认0X7F (亮度设置,越大越亮)）
	OLED_WR_Byte(0xa1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xc0,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display（设置显示方式;bit0:1,反相显示;0,正常显示）
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)（设置驱动路数）
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}
