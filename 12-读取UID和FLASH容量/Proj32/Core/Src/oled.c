
#include "oled.h"
#include "oledfont.h"  	

I2C_HandleTypeDef hi2c1;
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    //ʹ��HAL�������
    HAL_I2C_Mem_Write(&hi2c1,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&IIC_Command,1,100);
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    //ʹ��HAL�������
    HAL_I2C_Mem_Write(&hi2c1,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&IIC_Data,1,100);
}
/**********************************************
// Write OLED
**********************************************/
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd) Write_IIC_Data(dat);
  else    Write_IIC_Command(dat);	
}


/********************************************************************
* OLED��ʼ��
* ΪOLED����������׼��
********************************************************************/			    
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  

/********************************************
* ����OLED
* OLED DISPLAY ON
********************************************/   
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/********************************************
* �ر�OLED
* OLED DISPLAY OFF
********************************************/ 
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

/********************************************
* OLED ���������κ���ʾ
********************************************/  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);//����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);  //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);  //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}
/********************************************
* OLED������ȫ����ʾ
********************************************/  
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);//����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);  //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);  //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //������ʾ
}

/********************************************
* ��������
********************************************/  
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}
/********************************************************************
* 0.96 OLED 128*64���أ���:128�� x 64��
* 64�б����ֳ���8ҳ���ʼ8��Ϊ��0ҳ�����8��Ϊ��7ҳ��
* 
* ��������Ϊ:��x�У�yҳ��ʼ��ʾһ���ַ�,Char_SizeΪ�����С��
*
* x:0-127(��)
* y:0-7  (ҳ)
* Char_Size: 16(�����СΪ��8�� x 16��)������(6�� x 8��)
* flag,������ʾ��Ĭ��Ϊ0��������ʾ��1��������ʾ��
********************************************************************/	
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 flag)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
		{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			{
				if(flag == 0) OLED_WR_Byte( F8X16[c*16+i],OLED_DATA);
				else          OLED_WR_Byte(~F8X16[c*16+i],OLED_DATA);				
			}			
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			{
				if(flag == 0) OLED_WR_Byte( F8X16[c*16+i+8],OLED_DATA);
				else          OLED_WR_Byte(~F8X16[c*16+i+8],OLED_DATA);				
			}
		}
		else 
		{	
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
			{
				if(flag == 0) OLED_WR_Byte( F6x8[c][i],OLED_DATA);
				else          OLED_WR_Byte(~F6x8[c][i],OLED_DATA);					
			}		
		}
}
/********************************************************************
* 
* ��������Ϊ:m��n�η�
* 
********************************************************************/	
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/********************************************************************
* 0.96 OLED 128*64���أ���:128�� x 64��
* 64�б����ֳ���8ҳ���ʼ8��Ϊ��0ҳ�����8��Ϊ��7ҳ��
* 
* ��������Ϊ:��x�У�yҳ��ʼ��ʾ���֡�
*
* x:0-127(��)
* y:0-7  (ҳ)
* len:���ֳ���
* Char_Size: 16(�����СΪ��8�� x 16��)������(6�� x 8��)
*
********************************************************************/	
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Char_Size,u8 flag)
{         	
	u8 t,temp,RowChar,CharSize;
	u8 enshow=0;	
	
	if(Char_Size==16)//�ж������С
	{
		RowChar = 8;
		CharSize= 16;
	}
	else
	{
		RowChar = 6;
		CharSize= 8;		
	}
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+RowChar*t,y,' ',CharSize,flag);//��һ����0����֮ǰ�ÿո����
				continue;
			}
			else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+RowChar*t,y,temp+'0',CharSize,flag); 
	}
} 
/********************************************************************
* 0.96 OLED 128*64���أ���:128�� x 64��
* 64�б����ֳ���8ҳ���ʼ8��Ϊ��0ҳ�����8��Ϊ��7ҳ��
* 
* ��������Ϊ:��x�У�yҳ��ʼ��ʾ�ַ�����
*
* x:0-127(��)
* y:0-7  (ҳ)
* *chr:�ַ����׵�ַ
* Char_Size: 16(�����СΪ��8�� x 16��)������(6�� x 8��)
*
********************************************************************/	
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size,u8 flag)
{
	u8 CharSize,RowChar,j=0;
	
	if(Char_Size==16)
	{
		RowChar = 8;
		CharSize= 16;
	}
	else
	{
		RowChar = 6;
		CharSize= 8;		
	}	
	
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],CharSize,flag);
		x+=RowChar;
		if(x>(120-RowChar))//��β����һ���֣�����
		{
			x=0;
			y+=2;
		}
			j++;
	}
}
/********************************************************************
* 0.96 OLED 128*64���أ���:128�� x 64��
* 64�б����ֳ���8ҳ���ʼ8��Ϊ��0ҳ�����8��Ϊ��7ҳ��
* 
* ��������Ϊ:��x�У�yҳ��ʼ��ʾ���֣����ִ�С16x16
*
* x:0-127(��)
* y:0-7  (ҳ)
* *chr:�ַ����׵�ַ
* Num:��Num�����֣����ֶ�����oledfont.h������HzK[]�С�
*
********************************************************************/	
void OLED_ShowCHinese(u8 x,u8 y,u8 Num,u8 flag)
{      			    
	u8 t,adder=0;
	
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				if(flag == 0) OLED_WR_Byte( Hzk[2*Num][t],OLED_DATA);
				else 					OLED_WR_Byte(~Hzk[2*Num][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				if(flag == 0) OLED_WR_Byte( Hzk[2*Num+1][t],OLED_DATA);
				else 					OLED_WR_Byte(~Hzk[2*Num+1][t],OLED_DATA);
				adder+=1;
      }					
}
/********************************************************************
* 0.96 OLED 128*64���أ���:128�� x 64��
* 64�б����ֳ���8ҳ���ʼ8��Ϊ��0ҳ�����8��Ϊ��7ҳ��
* 
* ��������Ϊ:��x0�У�y0ҳ��ʼ��ʾͼƬ������
*
* x:0-127(��)
* y:0-7  (ҳ)
* *chr:�ַ����׵�ַ
* BMP[]:ͼƬ��ģ����
*
********************************************************************/	
void OLED_DrawBMP(u8 x0,u8 y0,u8 BMP[],u8 flag)
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
	for(y=y0;y<8;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<128;x++)
	    {      
	    	if(flag == 0) OLED_WR_Byte( BMP[j++],OLED_DATA);	
				else					OLED_WR_Byte(~BMP[j++],OLED_DATA);			
	    }
	}
} 






























