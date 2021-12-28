#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal_spi.h"
//#include "board.h"
/* ???????? ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

typedef  uint32_t  u32;
typedef  uint8_t  u8;// ???
typedef  uint16_t  u16;

#define SPI_FLASH_REBUILD           0    //1:???????????Flash??0??????????????Flash
#define SPI_FLASH_SECTOR_SIZE    4096    // ????Flash????????
#define SPI_FLASH_START_SECTOR   256*4    // ????Flash?????FatFS?????
#define SPI_FLASH_SECTOR_COUNT   256   // ????Flash?????FatFS???????????
//W25X???/Q??????????
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
//W25Q256 ID  0XEF18
//#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
//#define W25Q256 0XEF18
//#define  sFLASH_ID                        0XEF4017     //W25Q64
#define  sFLASH_ID 0XEF16
extern SPI_HandleTypeDef hspi1;
extern u16 W25QXX_TYPE;					//????W25QXX??????

#define W25QXX_CS_1		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1)
#define W25QXX_CS_0	      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0)
//////////////////////////////////////////////////////////////////////////////////
//????
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg1		0x05
#define W25X_ReadStatusReg2		0x35
#define W25X_ReadStatusReg3		0x15
#define W25X_WriteStatusReg1    0x01
#define W25X_WriteStatusReg2    0x31
#define W25X_WriteStatusReg3    0x11
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//???FLASH ID
void W25QXX_WAKEUP(void);				//????
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
#endif /* __SPI_FLASH_H */

