
#include "bsp_spi_flash.h"
//��Ҫ������Ӵ���
#include "stm32f1xx_hal_gpio.h"
//#include "board.h"
u16 W25QXX_TYPE=W25Q64;	//Ĭ����W25Q256
SPI_HandleTypeDef hspi1;


u8 W25QXX_ReadSR(u8 regno);             //??????????
void W25QXX_4ByteAddr_Enable(void);     //???4???????
void W25QXX_Write_SR(u8 regno,u8 sr);   //?????????
void W25QXX_Write_Enable(void);  		//?????
void W25QXX_Write_Disable(void);		//??????
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //???flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//????flash
void W25QXX_Erase_Chip(void);    	  	//???????
void W25QXX_Erase_Sector(u32 Dst_Addr);	//????????
void W25QXX_Wait_Busy(void);           	//???????
void W25QXX_PowerDown(void);        	//?????????

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 10);
    return Rxdata;          		    //�����յ�������
}

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    __HAL_SPI_DISABLE(&hspi1);            //�ر�SPI
    hspi1.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    hspi1.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    __HAL_SPI_ENABLE(&hspi1);             //ʹ��SPI

}

//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{
    u8 temp;
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           //ʹ��GPIOBʱ��
    // __HAL_RCC_GPIOA_CLK_ENABLE();           //ʹ��GPIOBʱ��

    //PB14
    GPIO_Initure.Pin=GPIO_PIN_12;          	//PB12
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����       
    //HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //��ʼ��

    W25QXX_CS_1;			                //SPI FLASH��ѡ��
//    SPI2_Init();		   			        //��ʼ��SPI
    SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2); //����Ϊ42Mʱ��,����ģʽ
    W25QXX_TYPE=W25QXX_ReadID();	        //��ȡFLASH ID.
    if(W25QXX_TYPE==W25Q64)                //SPI FLASHΪW25Q256
    {
        temp=W25QXX_ReadSR(3);              //��ȡ״̬�Ĵ���3���жϵ�ַģʽ
        if((temp&0X01)==0)			        //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
        {
            W25QXX_CS_0; 			        //ѡ��
            SPI2_ReadWriteByte(W25X_Enable4ByteAddr);//���ͽ���4�ֽڵ�ַģʽָ��   
            W25QXX_CS_1;       		        //ȡ��Ƭѡ   
        }
    }
}
//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
u8 W25QXX_ReadSR(u8 regno)
{
    u8 byte=0,command=0;
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_ReadStatusReg1;
            break;
    }
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(command);            //���Ͷ�ȡ״̬�Ĵ�������
    byte=SPI2_ReadWriteByte(0Xff);          //��ȡһ���ֽ�
    W25QXX_CS_1;                            //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(u8 regno,u8 sr)
{
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_WriteStatusReg1;
            break;
    }
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(command);            //����дȡ״̬�Ĵ�������
    SPI2_ReadWriteByte(sr);                 //д��һ���ֽ�
    W25QXX_CS_1;                            //ȡ��Ƭѡ
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_WriteEnable);   //����дʹ��
    W25QXX_CS_1;                            //ȡ��Ƭѡ
}
//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��
    W25QXX_CS_1;                            //ȡ��Ƭѡ
}

//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;
    W25QXX_CS_0;
    SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    Temp|=SPI2_ReadWriteByte(0xFF)<<8;
    Temp|=SPI2_ReadWriteByte(0xFF);
    W25QXX_CS_1;
    return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
    u16 i;
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_ReadData);      //���Ͷ�ȡ����
//    if(W25QXX_TYPE==W25Q64)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
//    {
//        SPI2_ReadWriteByte((u8)((ReadAddr)>>24));
//    }
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));   //����24bit��ַ
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));
    SPI2_ReadWriteByte((u8)ReadAddr);
    for(i=0;i<NumByteToRead;i++)
    {
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);    //ѭ������
    }
    W25QXX_CS_1;
}
/**
* @brief  ��ȡFLASH����
* @param 	pBuffer���洢�������ݵ�ָ��
* @param   ReadAddr����ȡ��ַ
* @param   NumByteToRead����ȡ���ݳ���
* @retval ��
*/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    /* ѡ��FLASH: CS�͵�ƽ */
    W25QXX_CS_0;

    /* ���� �� ָ�� */
    SPI2_ReadWriteByte(W25X_ReadData);

    /* ���� �� ��ַ��λ */
    SPI2_ReadWriteByte((ReadAddr & 0xFF0000) >> 16);
    /* ���� �� ��ַ��λ */
    SPI2_ReadWriteByte((ReadAddr& 0xFF00) >> 8);
    /* ���� �� ��ַ��λ */
    SPI2_ReadWriteByte(ReadAddr & 0xFF);

    /* ��ȡ���� */
    while (NumByteToRead--) /* while there is data to be read */
    {
        /* ��ȡһ���ֽ�*/
        *pBuffer = SPI2_ReadWriteByte(0xFF);
        /* ָ����һ���ֽڻ����� */
        pBuffer++;
    }

    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    W25QXX_CS_1;
}
#define WIP_Flag                  0x01

/**
 * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
 * @param  none
 * @retval none
 */
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0;

    /* ѡ�� FLASH: CS �� */
    W25QXX_CS_0;

    /* ���� ��״̬�Ĵ��� ���� */
    SPI2_ReadWriteByte(W25X_ReadStatusReg1);

    /* ��FLASHæµ����ȴ� */
    do
    {
        /* ��ȡFLASHоƬ��״̬�Ĵ��� */
        FLASH_Status = SPI2_ReadWriteByte(0xFF);
    }
    while ((FLASH_Status & WIP_Flag) == SET);  /* ����д���־ */

    /* ֹͣ�ź�  FLASH: CS �� */
    W25QXX_CS_1;
}


/**
 * @brief  ����FLASH����
 * @param  SectorAddr��Ҫ������������ַ
 * @retval ��
 */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{

    /* ����FLASHдʹ������ */
    W25QXX_Write_Enable();
    SPI_FLASH_WaitForWriteEnd();
    /* �������� */
    /* ѡ��FLASH: CS�͵�ƽ */
    W25QXX_CS_0;
    /* ������������ָ��*/
    SPI2_ReadWriteByte(W25X_SectorErase);
    /*���Ͳ���������ַ�ĸ�λ*/
    SPI2_ReadWriteByte((u8)((SectorAddr)>>16));   //����24bit��ַ
    SPI2_ReadWriteByte((u8)((SectorAddr)>>8));
    /* ���Ͳ���������ַ�ĵ�λ */
    SPI2_ReadWriteByte(SectorAddr & 0xFF);
    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    W25QXX_CS_1;
    /* �ȴ��������*/
    SPI_FLASH_WaitForWriteEnd();
}
/* WIP(busy)��־��FLASH�ڲ�����д�� */


#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
/**
 * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param	pBuffer��Ҫд�����ݵ�ָ��
 * @param WriteAddr��д���ַ
 * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
 * @retval ��
 */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /* ����FLASHдʹ������ */
    W25QXX_Write_Enable();

    /* ѡ��FLASH: CS�͵�ƽ */
    W25QXX_CS_0;
    /* дҳдָ��*/
    SPI2_ReadWriteByte(W25X_PageProgram);
    /*����д��ַ�ĸ�λ*/
    SPI2_ReadWriteByte((WriteAddr & 0xFF0000) >> 16);
    /*����д��ַ����λ*/
    SPI2_ReadWriteByte((WriteAddr & 0xFF00) >> 8);
    /*����д��ַ�ĵ�λ*/
    SPI2_ReadWriteByte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PageSize)
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        //FLASH_ERROR("SPI_FLASH_PageWrite too large!");
    }

    /* д������*/
    while (NumByteToWrite--)
    {
        /* ���͵�ǰҪд����ֽ����� */
        SPI2_ReadWriteByte(*pBuffer);
        /* ָ����һ�ֽ����� */
        pBuffer++;
    }

    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    W25QXX_CS_1;

    /* �ȴ�д�����*/
    SPI_FLASH_WaitForWriteEnd();
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u16 i;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_PageProgram);   //����дҳ����
//    if(W25QXX_TYPE==W25Q64)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
//    {
//        SPI2_ReadWriteByte((u8)((WriteAddr)>>24));
//    }
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));
    SPI2_ReadWriteByte((u8)WriteAddr);
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��
    W25QXX_CS_1;                            //ȡ��Ƭѡ
    SPI_FLASH_WaitForWriteEnd();					   //�ȴ�д�����
}


/**
 * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param	pBuffer��Ҫд�����ݵ�ָ��
 * @param  WriteAddr��д���ַ
 * @param  NumByteToWrite��д�����ݳ���
 * @retval ��
 */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
    Addr = WriteAddr % SPI_FLASH_PageSize;

    /*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
    count = SPI_FLASH_PageSize - Addr;
    /*�����Ҫд��������ҳ*/
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    /*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
    if (Addr == 0)
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*�Ȱ�����ҳ��д��*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*���ж���Ĳ���һҳ�����ݣ�����д��*/
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
        /* ����ַ�� SPI_FLASH_PageSize ������  */
    else
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            /*��ǰҳʣ���count��λ�ñ�NumOfSingleС��һҳд����*/
            if (NumOfSingle > count)
            {
                temp = NumOfSingle - count;
                /*��д����ǰҳ*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

                WriteAddr +=  count;
                pBuffer += count;
                /*��дʣ�������*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*��ַ����������count�ֿ������������������*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

            /* ���������ظ���ַ�������� */
            WriteAddr +=  count;
            pBuffer += count;
            /*������ҳ��д��*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*���ж���Ĳ���һҳ�����ݣ�����д��*/
            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}




//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
    while(1)
    {
        W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
        if(NumByteToWrite==pageremain)break;//д�������
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;

            NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
            if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
            else pageremain=NumByteToWrite; 	  //����256���ֽ���
        }
    };
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    W25QXX_BUF=W25QXX_BUFFER;
    secpos=WriteAddr/4096;//������ַ
    secoff=WriteAddr%4096;//�������ڵ�ƫ��
    secremain=4096-secoff;//����ʣ��ռ��С
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
    while(1)
    {
        W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
        for(i=0;i<secremain;i++)//У������
        {
            if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����
        }
        if(i<secremain)//��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);//�����������
            for(i=0;i<secremain;i++)	   //����
            {
                W25QXX_BUF[i+secoff]=pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������

        }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
        if(NumByteToWrite==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff=0;//ƫ��λ��Ϊ0

            pBuffer+=secremain;  //ָ��ƫ��
            WriteAddr+=secremain;//д��ַƫ��
            NumByteToWrite-=secremain;				//�ֽ����ݼ�
            if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
            else secremain=NumByteToWrite;			//��һ����������д����
        }
    };
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������
    W25QXX_CS_1;                            //ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //����falsh�������,������
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_SectorErase);   //������������ָ��
    if(W25QXX_TYPE==W25Q64)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI2_ReadWriteByte((u8)((Dst_Addr)>>24));
    }
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));
    SPI2_ReadWriteByte((u8)Dst_Addr);
    W25QXX_CS_1;                            //ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				    //�ȴ��������
}
//�ȴ�����
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1)&0x01)==0x01);   // �ȴ�BUSYλ���
}
//�������ģʽ
void W25QXX_PowerDown(void)
{
    W25QXX_CS_0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_PowerDown);     //���͵�������
    W25QXX_CS_1;                            //ȡ��Ƭѡ
    //delay_us(3);                            //�ȴ�TPD
}
//����
void W25QXX_WAKEUP(void)
{
    W25QXX_CS_0;                                //ʹ������
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB
    W25QXX_CS_1;                                //ȡ��Ƭѡ
    //delay_us(3);                                //�ȴ�TRES1
}

