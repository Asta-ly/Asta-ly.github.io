#include "bit_band.h"
#include "usart.h"	  

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	/* 发送一个字节 */
	USART_SendData(USART1,ch);

	/* 等待发送完毕 */
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);

	/* 返回发送的字符 */
	return ch;
}


 /*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}



#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
void uart_init(u32 bound)
{
   USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
 
	/* Enable GPIOA clock 使能GPIO端口A硬件时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable UART1 clock 使能硬件USART1的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Connect PA9 to USART1_Tx  将GPIO端口A的9号引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/* Connect PA10 to USART1_Rx 将GPIO端口A的10号引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* 配置GPIO端口A的第9和10引脚为推挽复用功能  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    //上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用功能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  //9和10引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //高速
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follows:
		 - BaudRate = 115200 baud  
		 - Word Length = 8 Bits
		 - One Stop Bit
		 - No parity
		 - Hardware flow control disabled (RTS and CTS signals)
		 - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = bound;   //通信双方必须保持一致
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* USART1 configuration */
	USART_Init(USART1, &USART_InitStructure);
	
	/* 配置NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //指定中断请求的通道：串口1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;   //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		   //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				   //指定中断请求的通道状态
	NVIC_Init(&NVIC_InitStructure);
	
	/* 开启串口接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
	printf("usart1_init_success\r\n");
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	






#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	  
void uart3_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART1);

	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    //上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用功能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //高速
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	USART_InitStructure.USART_BaudRate = bound;   //通信双方必须保持一致
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART3, &USART_InitStructure);
	
	/* 配置NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;               //指定中断请求的通道：串口1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;   //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		   //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				   //指定中断请求的通道状态
	NVIC_Init(&NVIC_InitStructure);
	
	/* 开启串口接收中断 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	/* Enable USART3 */
	USART_Cmd(USART3, ENABLE);
	printf("usart3_init_success\r\n");
}

#endif	

