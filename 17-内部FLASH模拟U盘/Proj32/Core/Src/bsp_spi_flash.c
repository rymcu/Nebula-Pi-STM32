
#include "bsp_spi_flash.h"
//需要重新添加代码
#include "stm32f1xx_hal_gpio.h"
//#include "board.h"
u16 W25QXX_TYPE=W25Q64;	//默认是W25Q256
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

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 10);
    return Rxdata;          		    //返回收到的数据
}

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&hspi1);            //关闭SPI
    hspi1.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    hspi1.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&hspi1);             //使能SPI

}

//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{
    u8 temp;
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           //使能GPIOB时钟
    // __HAL_RCC_GPIOA_CLK_ENABLE();           //使能GPIOB时钟

    //PB14
    GPIO_Initure.Pin=GPIO_PIN_12;          	//PB12
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速       
    //HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //初始化

    W25QXX_CS_1;			                //SPI FLASH不选中
//    SPI2_Init();		   			        //初始化SPI
    SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2); //设置为42M时钟,高速模式
    W25QXX_TYPE=W25QXX_ReadID();	        //读取FLASH ID.
    if(W25QXX_TYPE==W25Q64)                //SPI FLASH为W25Q256
    {
        temp=W25QXX_ReadSR(3);              //读取状态寄存器3，判断地址模式
        if((temp&0X01)==0)			        //如果不是4字节地址模式,则进入4字节地址模式
        {
            W25QXX_CS_0; 			        //选中
            SPI2_ReadWriteByte(W25X_Enable4ByteAddr);//发送进入4字节地址模式指令   
            W25QXX_CS_1;       		        //取消片选   
        }
    }
}
//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
u8 W25QXX_ReadSR(u8 regno)
{
    u8 byte=0,command=0;
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;
            break;
    }
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(command);            //发送读取状态寄存器命令
    byte=SPI2_ReadWriteByte(0Xff);          //读取一个字节
    W25QXX_CS_1;                            //取消片选
    return byte;
}
//写W25QXX状态寄存器
void W25QXX_Write_SR(u8 regno,u8 sr)
{
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;
            break;
    }
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(command);            //发送写取状态寄存器命令
    SPI2_ReadWriteByte(sr);                 //写入一个字节
    W25QXX_CS_1;                            //取消片选
}
//W25QXX写使能
//将WEL置位
void W25QXX_Write_Enable(void)
{
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_WriteEnable);   //发送写使能
    W25QXX_CS_1;                            //取消片选
}
//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(void)
{
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令
    W25QXX_CS_1;                            //取消片选
}

//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
//0XEF18,表示芯片型号为W25Q256
u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;
    W25QXX_CS_0;
    SPI2_ReadWriteByte(0x90);//发送读取ID命令
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    Temp|=SPI2_ReadWriteByte(0xFF)<<8;
    Temp|=SPI2_ReadWriteByte(0xFF);
    W25QXX_CS_1;
    return Temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
    u16 i;
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_ReadData);      //发送读取命令
//    if(W25QXX_TYPE==W25Q64)                //如果是W25Q256的话地址为4字节的，要发送最高8位
//    {
//        SPI2_ReadWriteByte((u8)((ReadAddr)>>24));
//    }
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));   //发送24bit地址
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));
    SPI2_ReadWriteByte((u8)ReadAddr);
    for(i=0;i<NumByteToRead;i++)
    {
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);    //循环读数
    }
    W25QXX_CS_1;
}
/**
* @brief  读取FLASH数据
* @param 	pBuffer，存储读出数据的指针
* @param   ReadAddr，读取地址
* @param   NumByteToRead，读取数据长度
* @retval 无
*/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    /* 选择FLASH: CS低电平 */
    W25QXX_CS_0;

    /* 发送 读 指令 */
    SPI2_ReadWriteByte(W25X_ReadData);

    /* 发送 读 地址高位 */
    SPI2_ReadWriteByte((ReadAddr & 0xFF0000) >> 16);
    /* 发送 读 地址中位 */
    SPI2_ReadWriteByte((ReadAddr& 0xFF00) >> 8);
    /* 发送 读 地址低位 */
    SPI2_ReadWriteByte(ReadAddr & 0xFF);

    /* 读取数据 */
    while (NumByteToRead--) /* while there is data to be read */
    {
        /* 读取一个字节*/
        *pBuffer = SPI2_ReadWriteByte(0xFF);
        /* 指向下一个字节缓冲区 */
        pBuffer++;
    }

    /* 停止信号 FLASH: CS 高电平 */
    W25QXX_CS_1;
}
#define WIP_Flag                  0x01

/**
 * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
 * @param  none
 * @retval none
 */
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0;

    /* 选择 FLASH: CS 低 */
    W25QXX_CS_0;

    /* 发送 读状态寄存器 命令 */
    SPI2_ReadWriteByte(W25X_ReadStatusReg1);

    /* 若FLASH忙碌，则等待 */
    do
    {
        /* 读取FLASH芯片的状态寄存器 */
        FLASH_Status = SPI2_ReadWriteByte(0xFF);
    }
    while ((FLASH_Status & WIP_Flag) == SET);  /* 正在写入标志 */

    /* 停止信号  FLASH: CS 高 */
    W25QXX_CS_1;
}


/**
 * @brief  擦除FLASH扇区
 * @param  SectorAddr：要擦除的扇区地址
 * @retval 无
 */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{

    /* 发送FLASH写使能命令 */
    W25QXX_Write_Enable();
    SPI_FLASH_WaitForWriteEnd();
    /* 擦除扇区 */
    /* 选择FLASH: CS低电平 */
    W25QXX_CS_0;
    /* 发送扇区擦除指令*/
    SPI2_ReadWriteByte(W25X_SectorErase);
    /*发送擦除扇区地址的高位*/
    SPI2_ReadWriteByte((u8)((SectorAddr)>>16));   //发送24bit地址
    SPI2_ReadWriteByte((u8)((SectorAddr)>>8));
    /* 发送擦除扇区地址的低位 */
    SPI2_ReadWriteByte(SectorAddr & 0xFF);
    /* 停止信号 FLASH: CS 高电平 */
    W25QXX_CS_1;
    /* 等待擦除完毕*/
    SPI_FLASH_WaitForWriteEnd();
}
/* WIP(busy)标志，FLASH内部正在写入 */


#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
/**
 * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
 * @retval 无
 */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /* 发送FLASH写使能命令 */
    W25QXX_Write_Enable();

    /* 选择FLASH: CS低电平 */
    W25QXX_CS_0;
    /* 写页写指令*/
    SPI2_ReadWriteByte(W25X_PageProgram);
    /*发送写地址的高位*/
    SPI2_ReadWriteByte((WriteAddr & 0xFF0000) >> 16);
    /*发送写地址的中位*/
    SPI2_ReadWriteByte((WriteAddr & 0xFF00) >> 8);
    /*发送写地址的低位*/
    SPI2_ReadWriteByte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PageSize)
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        //FLASH_ERROR("SPI_FLASH_PageWrite too large!");
    }

    /* 写入数据*/
    while (NumByteToWrite--)
    {
        /* 发送当前要写入的字节数据 */
        SPI2_ReadWriteByte(*pBuffer);
        /* 指向下一字节数据 */
        pBuffer++;
    }

    /* 停止信号 FLASH: CS 高电平 */
    W25QXX_CS_1;

    /* 等待写入完毕*/
    SPI_FLASH_WaitForWriteEnd();
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u16 i;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_PageProgram);   //发送写页命令
//    if(W25QXX_TYPE==W25Q64)                //如果是W25Q256的话地址为4字节的，要发送最高8位
//    {
//        SPI2_ReadWriteByte((u8)((WriteAddr)>>24));
//    }
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));
    SPI2_ReadWriteByte((u8)WriteAddr);
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//循环写数
    W25QXX_CS_1;                            //取消片选
    SPI_FLASH_WaitForWriteEnd();					   //等待写入结束
}


/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param  WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度
 * @retval 无
 */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
    Addr = WriteAddr % SPI_FLASH_PageSize;

    /*差count个数据值，刚好可以对齐到页地址*/
    count = SPI_FLASH_PageSize - Addr;
    /*计算出要写多少整数页*/
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    /*mod运算求余，计算出剩余不满一页的字节数*/
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if (Addr == 0)
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*先把整数页都写了*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
        /* 若地址与 SPI_FLASH_PageSize 不对齐  */
    else
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
            if (NumOfSingle > count)
            {
                temp = NumOfSingle - count;
                /*先写满当前页*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

                WriteAddr +=  count;
                pBuffer += count;
                /*再写剩余的数据*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

            /* 接下来就重复地址对齐的情况 */
            WriteAddr +=  count;
            pBuffer += count;
            /*把整数页都写了*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}




//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain=256-WriteAddr%256; //单页剩余的字节数
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
    while(1)
    {
        W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
        if(NumByteToWrite==pageremain)break;//写入结束了
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;

            NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
            if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
            else pageremain=NumByteToWrite; 	  //不够256个字节了
        }
    };
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    W25QXX_BUF=W25QXX_BUFFER;
    secpos=WriteAddr/4096;//扇区地址
    secoff=WriteAddr%4096;//在扇区内的偏移
    secremain=4096-secoff;//扇区剩余空间大小
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
    while(1)
    {
        W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
        for(i=0;i<secremain;i++)//校验数据
        {
            if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除
        }
        if(i<secremain)//需要擦除
        {
            W25QXX_Erase_Sector(secpos);//擦除这个扇区
            for(i=0;i<secremain;i++)	   //复制
            {
                W25QXX_BUF[i+secoff]=pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区

        }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(NumByteToWrite==secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff=0;//偏移位置为0

            pBuffer+=secremain;  //指针偏移
            WriteAddr+=secremain;//写地址偏移
            NumByteToWrite-=secremain;				//字节数递减
            if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
            else secremain=NumByteToWrite;			//下一个扇区可以写完了
        }
    };
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令
    W25QXX_CS_1;                            //取消片选
    W25QXX_Wait_Busy();   				   //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //监视falsh擦除情况,测试用
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_SectorErase);   //发送扇区擦除指令
    if(W25QXX_TYPE==W25Q64)                //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        SPI2_ReadWriteByte((u8)((Dst_Addr)>>24));
    }
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));
    SPI2_ReadWriteByte((u8)Dst_Addr);
    W25QXX_CS_1;                            //取消片选
    W25QXX_Wait_Busy();   				    //等待擦除完成
}
//等待空闲
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1)&0x01)==0x01);   // 等待BUSY位清空
}
//进入掉电模式
void W25QXX_PowerDown(void)
{
    W25QXX_CS_0;                            //使能器件
    SPI2_ReadWriteByte(W25X_PowerDown);     //发送掉电命令
    W25QXX_CS_1;                            //取消片选
    //delay_us(3);                            //等待TPD
}
//唤醒
void W25QXX_WAKEUP(void)
{
    W25QXX_CS_0;                                //使能器件
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB
    W25QXX_CS_1;                                //取消片选
    //delay_us(3);                                //等待TRES1
}

