#ifndef __W25Q128_H__
#define __W25Q128_H__

#include "stm32f4xx.h"



/* Ö¸Áî±í */
#define FLASH_ManufactDeviceID                  0x90
#define FLASH_WriteEnable                       0x06
#define FLASH_ReadStatusReg1                    0x05
#define FLASH_ReadData                          0x03
#define FLASH_PageProgram                       0x02
#define FLASH_SectorErase                       0x20
#define FLASH_DummyByte                         0xFF


void W25Qxx_Init(void);
uint8_t SPI_TransmissionByte(uint8_t send_data);
uint16_t W25Qxx_Read_ID(void);
void W25Qxx_ReadData(uint32_t addr, uint8_t *data, uint32_t data_len);
// 写使能：解锁
void W25Qxx_UnLock(void);
// 写失能
void W25Qxx_Lock(void);
// 读状态寄存器1
uint8_t W25Qxx_Read_Status1(void);	
void W25Qxx_Erase_Sector(uint32_t addr);
void W25Qxx_WriteData(uint32_t addr, uint8_t *data, uint32_t data_len);

#endif
