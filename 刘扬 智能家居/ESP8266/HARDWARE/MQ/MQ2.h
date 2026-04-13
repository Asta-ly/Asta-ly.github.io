#ifndef _MQ2_H_
#define _MQ2_H_

#include "stm32f4xx.h"
#include "ADC.h"
#include "delay.h"
#include "math.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	MQ-2烟雾传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.21
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

#define MQ2_READ_TIMES	10  //MQ-2传感器ADC循环读取次数
		

/*********************END**********************/


void MQ2_Init(void);
uint16_t MQ2_GetData(void);
float MQ2_GetData_PPM(void);
#endif
