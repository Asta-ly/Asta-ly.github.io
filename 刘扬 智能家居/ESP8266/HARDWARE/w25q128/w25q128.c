#include "w25q128.h"
#include "delay.h"
#include "bit_band.h"
#include "usart.h"


#include <string.h>

#define W25Qxx_CS    PBout(14)
#define W25Qxx_SCK   PBout(3)
#define W25Qxx_MOSI  PBout(5)
#define W25Qxx_MISO  PDin(4)

void W25Qxx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIOB clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* M4还未真正与从机通讯，片选引脚设置为高电平 */
	W25Qxx_CS = 1;
	
	/* 模式3空闲时时钟为高电平 */
	W25Qxx_SCK = 1;
	
	/* MOSI:电平随意 */
	W25Qxx_MOSI = 1;
	
}

uint8_t SPI_TransmissionByte(uint8_t send_data)
{
	uint8_t recv_data=0;
	
	for(uint8_t i=0; i<8; i++)
	{
		if(send_data & 1<<(7-i))  // bit 0 低电平  bit 1 高电平 
			W25Qxx_MOSI = 1;
		else
			W25Qxx_MOSI = 0;
		
		W25Qxx_SCK = 0; 
		delay_us(1);
		
		W25Qxx_SCK = 1; 
		delay_us(1);
	
		// 读取数据
		if(W25Qxx_MISO)
			recv_data |= 1<<(7-i);
	}
	
	return recv_data;
}

uint16_t W25Qxx_Read_ID(void)
{
	uint16_t id=0;
	
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x90
	SPI_TransmissionByte(0x90);
	// 发送地址码： 0x000000
	SPI_TransmissionByte(0x00);
	SPI_TransmissionByte(0x00);
	SPI_TransmissionByte(0x00);
	
	// 读厂商ID
	id = SPI_TransmissionByte(0x00)<<8;
	// 读设备ID
	id |= SPI_TransmissionByte(0x00);
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
	
	return id;
}

void W25Qxx_ReadData(uint32_t addr, uint8_t *data, uint32_t data_len)
{
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x03
	SPI_TransmissionByte(0x03);
	
	// 发送24bit地址
	SPI_TransmissionByte((addr>>16)&0xFF); // (0x123456 >> 16) & 0xFF 0x12
	SPI_TransmissionByte((addr>>8)&0xFF);  // 0x34
	SPI_TransmissionByte(addr&0xFF);       // 0x56
	
	for(uint32_t i=0; i<data_len; i++)
		data[i] = SPI_TransmissionByte(0xFF);
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
}

// 写使能：解锁
void W25Qxx_UnLock(void)
{
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x06
	SPI_TransmissionByte(0x06);
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
}

// 写失能
void W25Qxx_Lock(void)
{
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x04
	SPI_TransmissionByte(0x04);
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
}

// 读状态寄存器1
uint8_t W25Qxx_Read_Status1(void)
{
	u8 status;
	
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x05
	SPI_TransmissionByte(0x05);
	
	status = SPI_TransmissionByte(0xFF);
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
	
	return status;
}	

void W25Qxx_Erase_Sector(uint32_t addr)
{
	W25Qxx_UnLock();
	delay_us(10); // 延时一会等待从机稳定
	
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x20
	SPI_TransmissionByte(0x20);
	
	SPI_TransmissionByte((addr>>16)&0xFF); // (0x123456 >> 16) & 0xFF 0x12
	SPI_TransmissionByte((addr>>8)&0xFF);  // 0x34
	SPI_TransmissionByte(addr&0xFF);       // 0x56
	
	// CS为高电平,开始擦除
	W25Qxx_CS = 1;
	
	/* 等待擦除完成 BUSY位是否置为0 */
	while(W25Qxx_Read_Status1() & 0x01)
	{
		delay_ms(1);
	}
	
	// 锁定
	W25Qxx_Lock();
}

void W25Qxx_WriteData(uint32_t addr, uint8_t *data, uint32_t data_len)
{
	/* 一次最多可编程256Byte */
	// 检查是否需要擦除：读取写入的地址判断是否为0xFF，不是则需要先擦除扇区
	// .......
	
	if(data_len >= 256)
	{
		printf("Out of range!\n");
		return;
	}
		
	// 解锁
	W25Qxx_UnLock();
	delay_us(10); // 延时一会等待从机稳定
	
	// 从机开始工作：CS为低电平
	W25Qxx_CS = 0;
	
	// 发送代码：0x02
	SPI_TransmissionByte(0x02);
	
	SPI_TransmissionByte((addr>>16)&0xFF); // (0x123456 >> 16) & 0xFF 0x12
	SPI_TransmissionByte((addr>>8)&0xFF);  // 0x34
	SPI_TransmissionByte(addr&0xFF);       // 0x56
	
	for(u8 i=0; i<data_len; i++)
		SPI_TransmissionByte(data[i]);
	/* 等待写入完成 */
	while(W25Qxx_Read_Status1() & 0x01)
	{
		delay_ms(1);
	}
	
	// 通讯结束：CS为高电平
	W25Qxx_CS = 1;
	// 上锁
	W25Qxx_Lock();
}

//int main()
//{
//	uint16_t ID;
//	u8 buf[6]={0};
//	
//	/* 中断优先级分组：只需配置一次 */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	/* 初始化串口1：波特率 115200 */
//	USART1_Config(115200);
//	memset((void *)usart1_recv_buf, 0,U1_MLEN);
//	
//	printf("this is read W25Q128 ID sample...\n");
//	
//	W25Qxx_Init();
//	
//	ID = W25Qxx_Read_ID();
//	
//	printf("厂商ID:%#x 设备ID:%#x\n", ID>>8, ID&0xFF);
//	
//	// 擦除扇区
//	W25Qxx_Erase_Sector(0x20000);
//	
//	u8 data[] = "hello";
//	
//	W25Qxx_WriteData(0x20000, data, 6);
//	
//	W25Qxx_ReadData(0x20000, buf, sizeof(buf));
//	printf("读取到0x200000地址数据是: ");
////	for(u8 i=0; i<sizeof(buf); i++)
////		printf("%c", buf[i]);
//	printf("%s\n", buf);
//	
//	while(1)
//	{
//		
//	}
//}
