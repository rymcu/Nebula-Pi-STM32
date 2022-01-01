/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "bsp_spi_flash.h"
#include "oled.h"
#include "bmp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t rx_buff[100];  //接收缓存
uint8_t rx_done = 0; //接收完成标志
uint8_t rx_cnt = 0;//接收数据长度
//IIC全局变量
#define ADDR_WR_AT24CXX 0xA0 //写器件地址
#define ADDR_RD_AT24CXX 0xA1 //读器件地址
#define BuffSize 256
uint8_t Wr_buff[BuffSize],Rd_buff[BuffSize];//读写数据buff
//CAN全局变量
CAN_TxHeaderTypeDef TxHeaderCAN;
CAN_RxHeaderTypeDef RxHeaderCAN;
uint8_t TxDataCAN[8],RxDataCAN[8];
uint8_t CAN_Rx_Flag=0;//CAN接收标志

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void EEPROM_IIC_Test(void);
void SPI_FLASH_Test(void);
void FATFS_FLASH_Test(void);
void OLED_IIC_Test(void);
void CAN_Test(void);
void filter_init(void);
void FLASH_Inside_Test(void);
void Get_MCU_Info(void);
void Get_Temp(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_CAN_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_CAN_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  //EEPROM测试
  //EEPROM_IIC_Test();
  //SPI_FLASH_Test();
  //FATFS_FLASH_Test();
  //OLED_IIC_Test();
  //filter_init();
  //CAN_Test();
  //FLASH_Inside_Test();
  //Get_MCU_Info();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      //采集温度
      Get_Temp();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
    //启动CAN
    if(HAL_CAN_Start(&hcan) != HAL_OK)
    {
        printf("CAN start Fail!\r\n");
    }
    //开启中断,FIFO 0接收消息中断
    HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  //开启接收中断，空闲中断
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE|UART_IT_RXNE);
  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Exti13_Pin */
  GPIO_InitStruct.Pin = Exti13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Exti13_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief 读取内部温度传感器
  */
void Get_Temp(void)
{
    uint32_t Temp;//温度采样分层值
    float Vsense = 0.0;//温度采样电压值
    float Temperature = 0.0;//温度值
    //数据手册温度转换公式：T = ((V25-Vsense)/Avg_Slope) + 25
    float V25 = 1.43;//查阅手册获得
    float Avg_Slope = 0.0043;//4.3mV/摄氏度
    printf("\r\n\r\n------------------MCU内部温度传感器测试------------------\r\n\r\n");
    //step1 启动ADC
    HAL_ADC_Start(&hadc1);
    //step2 温度采集转换
    HAL_ADC_PollForConversion(&hadc1,5);
    //step3 转换计算
    Temp = HAL_ADC_GetValue(&hadc1);//获取采样值分层值
    Vsense = Temp *(3.3/4096);//采样精度12bit,最大分层值4096
    Temperature = ((V25-Vsense)/Avg_Slope) + 25;//按公式计算温度值
    //step4 串口打印

    printf("温度分层值：%d\r\n温度电压值：%0.3f\r\n温度采样值：%0.3f\r\n",Temp,Vsense,Temperature);

    HAL_Delay(1000);
}
/**
  * @brief 获取MCU设备信息
  */
void Get_MCU_Info(void)
{
    printf("\r\n\r\n------------------获取单片机设备信息------------------\r\n\r\n");
    uint32_t  FLASH_Size_Addr = 0x1FFFF7E0;//FLASH大小存储地址
    //获取存储器大小
    printf("     FLASH  Size: %dKB\r\n",*(uint16_t *)FLASH_Size_Addr);
    //获取HAL版本
    uint32_t HALVer = HAL_GetHalVersion();
    printf("    HAL  Version: V%d.%d.%d\r\n",HALVer>>24,(HALVer>>16)&0xFF,(HALVer>>8)&0xFF);
   //获取保留ID？
    printf("    Revision    ID: 0x%X\r\n",HAL_GetREVID());
    //获取全球唯一UID
    printf("Unique ID(UID): 0x%08X %08X %08X\r\n",HAL_GetUIDw0(),HAL_GetUIDw1(),HAL_GetUIDw2());

    //获取设备ID
    printf("Device ID or Chip ID or Product ID(PID): 0x%x\r\n",HAL_GetDEVID());

}
/**
  * @brief 往内部FLASH写入数据
  * @param addr 写入地址
  * @param pdata 存储待写数据
  * @retval None
  */
void FLASH_Inside_Wr(uint32_t addr,uint32_t Pdata)
{
    //定义局部变量
    uint32_t PageError = 0;
    HAL_StatusTypeDef HAL_Status;
    //擦除配置信息结构体，包括擦除地址、方式、页数等
    FLASH_EraseInitTypeDef pEraseInit;
    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;//按页擦除
    pEraseInit.PageAddress = addr;//擦除地址
    pEraseInit.NbPages = 1;//擦除页数量
    //step1 解锁内部FLASH，允许读写功能
    HAL_FLASH_Unlock();
    //step2 开始擦除addr对应页
    HAL_Status = HAL_FLASHEx_Erase(&pEraseInit,&PageError);//擦除
    if(HAL_Status != HAL_OK) printf("内部FlASH擦除失败！\r\n");
    //step3 写入数据
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,Pdata);
    //step4 锁定FLASH
    HAL_FLASH_Lock();
}
/**
  * @brief 读取内部FLASH数据
  * @param addr 读取地址
  * @retval 读出的数据
  */
uint32_t FLASH_Inside_Rd(uint32_t addr)
{
    //定义局部变量
    uint32_t RdData = 0;
    //step1 解锁内部FLASH，允许读写功能
    HAL_FLASH_Unlock();
    RdData = *(__IO uint32_t *)addr;
    //step2 锁定FLASH
    HAL_FLASH_Lock();

    return RdData;
}
/**
  * @brief 内部FLASH读写测试
  */
void FLASH_Inside_Test(void)
{
    printf("\r\n\r\n------------------内部FLASH读写测试------------------\r\n\r\n");
    uint32_t addr = 0x08010000;//确保该地址内部FLASH是空余的！
    uint32_t WrData = 0x01234567;//待写入数据
    uint32_t RdData = 0;//存储读取数据

    printf("地址0x%x写入数据：0x%x\r\n",addr,WrData);
    FLASH_Inside_Wr(addr,WrData);//写入数据
    RdData = FLASH_Inside_Rd(addr);//读取数据
    printf("地址0x%x读出数据：0x%x\r\n",addr,RdData);
}

void filter_init(void)
{
    HAL_StatusTypeDef HAL_Status;
    CAN_FilterTypeDef Filter0;
    Filter0.FilterBank = 1;//滤波器编号
    Filter0.FilterMode = CAN_FILTERMODE_IDMASK;
    Filter0.FilterScale = CAN_FILTERSCALE_32BIT;
    Filter0.FilterIdHigh = 0x00;
    Filter0.FilterIdLow = 0x00;
    Filter0.FilterMaskIdHigh = 0x00;
    Filter0.FilterMaskIdLow = 0x00;
    Filter0.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    Filter0.FilterActivation = CAN_FILTER_ENABLE;

    HAL_Status = HAL_CAN_ConfigFilter(&hcan,&Filter0);
    if(HAL_Status != HAL_OK)
    {
        printf("CAN Filter set Fail!code:%d\r\n",HAL_Status);
        Error_Handler();
    }
}
void CAN_Test(void)
{
    //发送数据CAN
    TxHeaderCAN.ExtId = 0x1800F001;
    TxHeaderCAN.DLC = 8;
    TxHeaderCAN.IDE = CAN_ID_STD;
    TxHeaderCAN.RTR = CAN_RTR_DATA;
    TxHeaderCAN.StdId = 0x01;
    TxHeaderCAN.TransmitGlobalTime = ENABLE;

    uint32_t TxMailBox;
    HAL_StatusTypeDef HAL_Status;
    printf("\r\n\r\n------------------CAN通信测试------------------\r\n\r\n");
    for (int i = 0; i < 8; ++i) TxDataCAN[i] = i;
    printf("CAN发送数据：\r\n");
    for (int i = 0; i < 8; ++i) printf(" 0x%02x",TxDataCAN[i]);
    printf("\r\n");
    HAL_CAN_AddTxMessage(&hcan,&TxHeaderCAN,TxDataCAN,&TxMailBox);
}
//重定义CAN接收中断回调函数
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    HAL_StatusTypeDef HAL_Status;
    if(hcan->Instance == CAN1)
    {
        HAL_Status = HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeaderCAN,RxDataCAN);
        if(HAL_Status == HAL_OK)
        {
            //处理接收数据
            CAN_Rx_Flag = 1;
        }
    }
}

//0.96 OLED测试
void OLED_IIC_Test(void)
{
    OLED_Init();//初始化OLED
    OLED_Clear();//清除屏幕
    OLED_Display_On();//开启OLED

/*****************************************
	*
	*0.96 OLED 字符显示测试
	*
*******************************************/
    OLED_ShowChar( 0,0,'A',16,0);
    OLED_ShowChar( 8,0,'B',16,0);
    OLED_ShowChar(16,0,'C',16,0);
    OLED_ShowChar(24,0,'D',16,0);

    OLED_ShowChar( 0,2,'A',8,0);
    OLED_ShowChar( 8,2,'B',8,0);
    OLED_ShowChar(16,2,'C',8,0);
    OLED_ShowChar(24,2,'D',8,0);

    OLED_ShowString(25,6,"Char Test!",16,1);

    HAL_Delay(5000);
    OLED_Clear();//清除屏幕

/*****************************************
	*
	*0.96 OLED 数字显示测试
	*
*******************************************/

    OLED_ShowNum(  0,1,12,2,16,0);
    OLED_ShowNum( 48,1,34,2,16,0);
    OLED_ShowNum( 96,1,56,2,16,0);

    OLED_ShowString(25,6,"Num Test!",16,1);

    HAL_Delay(5000);
    OLED_Clear();//清除屏幕

/*****************************************
	*
	*0.96 OLED 中文显示测试
	*
*******************************************/
    OLED_ShowCHinese(22   ,3,1,0);//不
    OLED_ShowCHinese(22+16,3,2,0);//见
    OLED_ShowCHinese(22+32,3,3,0);//不
    OLED_ShowCHinese(22+48,3,4,0);//散
    OLED_ShowCHinese(22+64,3,5,0);//！

    OLED_ShowString(25,6,"CHN Test!",16,1);

    HAL_Delay(5000);
    OLED_Clear();//清除屏幕

/*****************************************
	*
	*0.96 OLED 字符串显示测试
	*
*******************************************/

    OLED_ShowString(0,2,"Nebula-Pi,RYMCU!",16,0);
    OLED_ShowString(25,6,"Str Test!",16,1);
    HAL_Delay(5000);
    OLED_Clear();//清除屏幕
/*****************************************
	*
	*0.96 OLED 图片显示测试
	*
*******************************************/

    OLED_DrawBMP(0,0,Logo,0);//显示图片
    OLED_ShowString(25,6,"PIC Test!",16,1);
    HAL_Delay(5000);
}
//FATFS测试
void FATFS_FLASH_Test(void)
{
    FATFS fs;//文件系统对象
    static FIL fnew;//文件对象
    BYTE FATFS_Wr_Buff[128]  ="hello,嵌入式知识开源社区RYMCU欢迎你！\r\n";// 写缓冲区
    BYTE FATFS_Rd_Buff[128]  ={0};// 读缓冲区
    UINT fnum;//成功读写数量
    FRESULT res;//返回值

    printf("\r\n\r\n------------------FLASH FATFS文件系统测试------------------\r\n\r\n");
    res = f_mount(&fs,"0:",1);
    if(res == FR_NO_FILESYSTEM)
    {
        printf("FLASH上没有建立文件系统，开始格式化！\r\n");
        res = f_mkfs("0:",0,0);//格式化
        if(res == FR_OK)
        {
            printf("FLASH文件系统格式化成功！\r\n");
            //格式化成功后先取消，再重新挂载！
            res = f_mount(NULL,"0:",1);
            printf("取消挂载成功！： %d\r\n",res);
            res = f_mount(&fs,"0:",1);
            printf("重新挂载成功！： %d\r\n",res);
        }
        else
        {
            printf("FLASH文件系统格式化失败！\r\n");
        }
    }
    printf("\r\n\r\n-------------------FATFS系统测试：写文件-------------------\r\n");
    //打开文件，文件不存在则创建并打开
    res = f_open(&fnew,"RY.txt",FA_CREATE_ALWAYS | FA_WRITE);
    if(res == FR_OK) printf("打开或创建RY.txt成功！\r\n");
    else             printf("打开或创建RY.txt失败！\r\n");
    //写测试
    res = f_write(&fnew,FATFS_Wr_Buff, sizeof(FATFS_Wr_Buff),&fnum);
    if(res == FR_OK) printf("往RY.txt写入数据：\r\n%s",FATFS_Wr_Buff);
    else             printf("往RY.txt写入数据失败,code: %d!\r\n",res);
    //完成写操作后，关闭文件
    f_close(&fnew);
    printf("\r\n-------------------FATFS系统测试：读文件-------------------\r\n\r\n");
    //打开文件，读方式打开已创建的文件
    res = f_open(&fnew,"RY.txt",FA_OPEN_EXISTING | FA_READ);
    if(res != FR_OK){printf("打开文件失败！\r\n");return;}
    //读测试
    res = f_read(&fnew,FATFS_Rd_Buff, sizeof(FATFS_Rd_Buff),&fnum);
    if(res != FR_OK){printf("读取文件失败！\r\n");return;}
    printf("读取文件数据：\r\n%s\r\n",FATFS_Rd_Buff);
    f_close(&fnew);//读取完毕，关闭文件。
}
//SPI FLASH测试
#define  FLASHBUFFSIZE 64
void SPI_FLASH_Test(void)
{
    uint16_t FLASH_ID;
    uint8_t Flash_Wr_Buff[FLASHBUFFSIZE],Flash_Rd_Buff[FLASHBUFFSIZE];
    printf("SPI总线通信实验，读写8M外部FLASH W25Q64!\r\n",FLASH_ID);
    FLASH_ID = W25QXX_ReadID();//读取W25Q64器件ID
    HAL_Delay(100);
    printf("the FLASH Device ID is 0x%X!\r\n",FLASH_ID);
    //W25Q64 = 8MByte,地址范围：0x000000-0x7FFFFF
    //Sector：每4KB为一个Sector,8M/4K=2048个
    //Block：每64KB为一个Block,8M/64K=128个，每个Block包括16个Sector
    SPI_FLASH_SectorErase(0x00);//擦除第一个Sector
    for (int i = 0; i < FLASHBUFFSIZE; ++i) Flash_Wr_Buff[i] = i;
    SPI_FLASH_BufferWrite(Flash_Wr_Buff,0x00,FLASHBUFFSIZE);
    SPI_FLASH_BufferRead(Flash_Rd_Buff,0x00,FLASHBUFFSIZE);
    printf("Read data form W25Q64: \r\n");
    for(uint16_t i=0;i<FLASHBUFFSIZE;i++)
    {
        printf("0x%02x ",Flash_Rd_Buff[i]);
    }
    printf("\r\n");
}
//EEPROM测试
void EEPROM_IIC_Test(void)
{
    HAL_StatusTypeDef error;
    uint8_t length = 8 ;
    for(uint16_t i = 0;i<256;i++) Wr_buff[i] = i;
    //AT24Cxx地址00开始写入8Byte数据
    error = HAL_I2C_Mem_Write(&hi2c1,ADDR_WR_AT24CXX,0x00,I2C_MEMADD_SIZE_16BIT,Wr_buff,length,1000);
    if(error == HAL_OK) printf("eeprom write done!\r\n");
    else                printf("eeprom write fail!\r\n");
    HAL_Delay(1000);
    //从地址0x00开始读8Byte数据，并打印。
    error = HAL_I2C_Mem_Read(&hi2c1,ADDR_RD_AT24CXX,0x00,I2C_MEMADD_SIZE_16BIT,Rd_buff,length,1000);
    if(error == HAL_OK)
    {
        printf("eeprom read done!\r\n");
        for(uint16_t i=0;i<length;i++)
        {
            printf("0x%02x ",Rd_buff[i]);
        }
        printf("\r\n");
    }
    else  printf("eeprom write fail!\r\n");
}

//重写GPIO中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //翻转LED0
    HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
    //打印中断处理信息
    printf("ExTi13 interrput!\r\n");
}




#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch. FILE *f)
#endif /* __GNUC__ */

//重定向printf函数
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);//输出指向串口USART1
    return ch;
}
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

