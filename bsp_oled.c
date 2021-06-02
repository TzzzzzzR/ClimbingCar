#include <stdio.h>
#include <stdlib.h>
#include "bsp_oled.h"
#include "bsp_oledfont.h"

//OLED���Դ�
//��Ÿ�ʽ����.��8ҳ
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
uint8_t OLED_GRAM[128][8];//һ����ά���飬�Ȳ���������飬�����һ����д��

#if OLED_MODE == 1
//��SSD1106д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
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
//��SSD1106д��һ���ֽڡ�

//�˴�Ϊ��IO�����ģ��SPI������Ҫ�鿴SPIʱ��ͼ��
//dat:Ҫд�������/����
//��SSD1106�ֲ᣺cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{	
	unsigned char i;
	OLED_CS_Clr();			//CS:Ƭѡ�ź��ߣ��½���ʼ��������
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  

	for(i=0;i<8;i++)		//ѭ������8λ��һ�ֽڣ�
	{			  
		OLED_SCLK_Clr();	//SCLK���ͣ��� SCLK �������أ����ݴ� SDIN ���뵽SSD1306
		if(dat&0x80)			//�������ݣ���dat��ÿһλ��½������SDIN����SSD1306��
		   OLED_SDIN_Set();	
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   				//��λ��ǰ���Ӹ������
	}				 		  
	OLED_CS_Clr();			//Ƭѡ�ź����������źŴ������
	OLED_DC_Set();   	  //Ĭ���ø�
} 
#endif
//�鿴SSD1306�ֲᣨ���㿪��ָ����Ҳ���ᣩ�ɵã�д�����������ܴﵽЧ��

//����ֱ���ö�ά���������ݸ�����(����OLED_Refresh_Gram)
//������ʼ��ַ�ĺ�����
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//0XB0~B7����������������ҳ��ַ�������λ��ֵ��Ӧ�� GRAM ��ҳ��ַ
	//�޷����������ƣ���Ϊ�߼����ƣ�����λ��0���з�����������λ�Ƹ�λ��1��
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//0X10~0X1F����ָ������������ʾʱ����ʼ�е�ַ����λ��
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); //0X00~0X0F����ָ������������ʾʱ����ʼ�е�ַ����λ
} 
	 

//�����Դ浽LCD���׵�ַ��Ϊ0,0. Ȼ����ͨ����ά����д������		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//����OLED��ʾ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����	������ɱ�����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON				��ɱÿ�
	OLED_WR_Byte(0XAF,OLED_CMD);  //������ʾ���أ�	o AEh : Display OFF o AFh : Display ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ�ģ���û����Ч��һ��	  
/*void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x02,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}*/
//��������棺
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;						//64�У�y����8ҳ��pos��
	bx=y%8;								//�õ�����һҳ�ĵڼ�λ
	temp=1<<(7-bx);				//ÿһҳ8λ����1�Ƶ�������λ��ע��64������ÿ8λ�ǴӸߵ��͵ģ���7-bx��
	if(t)OLED_GRAM[x][pos]|=temp;//OLED_GRAM[x][pos]�õ�����һ��8λ�����ݣ�Ҫ��������
	else OLED_GRAM[x][pos]&=~temp;	    
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);	//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ	���������������������������������������������û����������������������������������������������
  for(t=0;t<csize;t++)
  { 
		//asc2_1206ΪASCII�ַ������󣨶�ά���飩
		//�����chr������asc2_1206[chr]����������t����������һ��
		
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;															//û�е��ֿ�
		
		for(t1=0;t1<8;t1++)//ÿһ��t��Ӧһ����λ������t1������
		{
			//ÿһλ����0/1��ʾ�����෴����������ASCII�ַ��������ԭ��
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)											
			{
				y=y0;
				x++;						//����һ��y��x++����һ��
				break;
			}
		}  	 
  }          
}


/*����һ��������ͬ������û����OLED_DrawPoint���β���һ�����𣩻����õ�ʱ��ǵø�
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
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
//m^n����
//һ���򵥵���ѧ��������ʾ����Ҫ��
unsigned long oled_pow(unsigned char m,unsigned char n)
{
	unsigned long result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)											//ѭ��һ��дһλ
	{
		temp=(num/oled_pow(10,len-t-1))%10;		//�õ�ÿһλ�������Ƕ���
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)													//���λΪ0ʱ
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);//���һ��' '
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); //temp+'0'Ϊ�����һ���ַ���ͨ��OLED_ShowChar�����
	}
} 

//�������õڶ���showchar��shounum
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
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}//����������sizeԭ�򳬹���ʾ��Χ�Ĵ���
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
}	 

/*(2)ͬ��
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


//��ʼ��SH1106					    
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

	//�ɲ����ֲ�ã�				  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel���ر���ʾ��
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address��[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;��
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)��������ʾ��ʼ�� [5:0],������
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register���Աȶ����ã�
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness��1~255;Ĭ��0X7F (��������,Խ��Խ��)��
	OLED_WR_Byte(0xa1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xc0,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display��������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ��
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)����������·����
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
