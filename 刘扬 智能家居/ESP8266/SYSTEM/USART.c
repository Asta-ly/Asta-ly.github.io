#include "USART.h"
#include "delay.h"

#include "bit_band.h"


#include "delay.h"



volatile uint16_t usart1_recv_data;
//usart2
volatile uint8_t usart2_recv_index;
volatile uint8_t usart2_recv_buf[U2_MLEN];


uint8_t usart2_data_ready = 0;
//usart1
volatile uint8_t usart1_recv_index;
volatile uint8_t usart1_recv_buf[U1_MLEN];
uint8_t usart1_data_ready = 0;

void USART1_Config(void)
{
	//配置io口，串口和中断
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//GPIOA  clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//USART1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	//RX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	/* GPIO config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	/* USART1 config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	//enable USART
//	USART_Cmd(USART1,ENABLE);
	
	/* 配置串口接收中断 */
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能串口接收中断 */
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//ENABLE USART
	USART_Cmd(USART1,ENABLE);
}

void USART2_Config(void)
{
	//配置io口，串口和中断
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//GPIOA  clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//USART1 clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	//RX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	/* GPIO config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
	/* USART1 config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	
	//enable USART
//	USART_Cmd(USART1,ENABLE);
	
	/* 配置串口接收中断 */
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能串口接收中断 */
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//ENABLE USART
	USART_Cmd(USART2,ENABLE);
}

void  USART_Send_Data(USART_TypeDef* USARTx, char *string)
{
	uint16_t i;
	for(i = 0; string[i] != '\0';i++)
	{
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx,string[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
	}

}



struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
 // return (ITM_SendChar(ch));
 // 换为串口1发送
	//等待发送寄存器空
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  // RESET不为空
	USART_SendData(USART1, ch);
	//等到发送完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    return ch;
}

void _sys_exit(int return_code) {
label: goto label; /* endless loop */
}


void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET )
	{
		// printf("USART2中断触发\n"); // 若能打印，说明中断正常
		usart2_recv_buf[usart2_recv_index++] = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		//USART_SendData(USART1, usart2_recv_buf[usart2_recv_index-1]);
		if(usart2_recv_buf[usart2_recv_index-1] == '#')
		{
			usart2_data_ready = 1;
			printf("access recv");
		}
	}
		
}

void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET )
	{
		usart2_recv_buf[usart1_recv_index++] = USART_ReceiveData(USART1);
		//USART_SendData(USART1, usart2_recv_buf[usart2_recv_index-1]);
		if(usart2_recv_buf[usart1_recv_index-1] == '#')
		{
			usart1_data_ready = 1;
			
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

