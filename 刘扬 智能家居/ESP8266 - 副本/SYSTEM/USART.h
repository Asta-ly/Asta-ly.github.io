#ifndef _USART_H_
#define _USART_H_
#include "stm32f4xx.h"

#pragma import(__use_no_semihosting_swi)
#include <stdio.h>
#include <stdlib.h>
#define U2_MLEN 512
#define U1_MLEN 128


extern void USART1_Config(void);
extern int fputc(int ch, FILE *f);
extern void _sys_exit(int return_code);
extern void  USART_Send_Data(USART_TypeDef* USARTx, char *string);
extern void USART2_Config(void);



extern volatile uint16_t usart1_recv_data; 
extern volatile uint8_t usart2_recv_index;
extern volatile uint8_t usart2_recv_buf[U2_MLEN];

extern volatile uint8_t usart1_recv_index;
extern volatile uint8_t usart1_recv_buf[U1_MLEN];
extern uint8_t usart2_data_ready;
extern uint8_t usart1_data_ready;
	
#endif

