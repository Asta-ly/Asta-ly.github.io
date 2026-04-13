#include "led.h"



void led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/* GPIOF and GPIOE Peripheral clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOE, ENABLE);
	
	/* Configure PF9 and PF10 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//以结构体的初始化配置GPIO端口F
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	/* Configure PE13 and PE14 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	//以结构体的初始化配置GPIO端口E
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	PFout(9)  = 1;
	PFout(10) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
	
}
