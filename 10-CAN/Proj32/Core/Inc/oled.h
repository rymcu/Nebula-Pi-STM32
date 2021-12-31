
#ifndef __OLED_H
#define __OLED_H		

#include "stm32f1xx_hal.h"
#define  u8 unsigned char 
#define  u32 unsigned int 

 extern I2C_HandleTypeDef hi2c1;
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
#define OLED_MODE 0

 
#define Max_Column	128  

//-----------------OLED�˿ڶ���----------------  					   

void delay_ms(unsigned int ms);


//OLED�����ú���  
void OLED_Init(void);//��ʼ��
void OLED_Display_On(void);//����OLED
void OLED_Display_Off(void);//�ر�OLED	   							   		    
void OLED_Clear(void);//����
void OLED_On(void);//OLED������ȫ����ʾ

void OLED_Set_Pos(unsigned char x, unsigned char y);//��������


void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 flag);//��ʾ�ַ�
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Char_Size,u8 flag);//��ʾ����
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size,u8 flag);//��ʾ�ַ���
void OLED_ShowCHinese(u8 x,u8 y,u8 Num,u8 flag);//��ʾ�����ַ�
void OLED_DrawBMP(u8 x0,u8 y0,u8 BMP[],u8 flag);//��ʾͼƬ



#endif  
	 



