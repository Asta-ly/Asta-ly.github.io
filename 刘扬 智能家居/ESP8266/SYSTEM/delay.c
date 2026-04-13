#include "delay.h"

void delay(unsigned int n)
{
	unsigned int k = 0x1200000;
	while(n--)
		while(k--);
}

void delay_us(uint32_t nus)
{
	SysTick->CTRL = 0;           // 关闭定时器
	SysTick->LOAD = (168-1)*nus; // 装置值为n，计数为n~0（n+1个数）
	SysTick->VAL = 0;            // 清除当前值寄存器
	SysTick->CTRL = 5;            // 启动定时器并选择HCLK作为时钟源 168MHz
	while ((SysTick->CTRL & 0x00010000)==0);// 等待计数值到达0  定时时间到达
	SysTick->CTRL = 0; // 关闭定时器
}

void delay_ms(uint32_t nms)
{
	while(nms--)
	{
		// delay_us(1000);
		SysTick->CTRL = 0;            // 关闭定时器
		SysTick->LOAD = (168000-1);   // 装置值为n，计数为n~0（n+1个数）
		SysTick->VAL = 0;             // 清除当前值寄存器
		SysTick->CTRL = 5;            // 启动定时器并选择HCLK作为时钟源 168MHz
		while ((SysTick->CTRL & 0x00010000)==0);// 等待计数值到达0  定时时间到达
		SysTick->CTRL = 0; // 关闭定时器
	}
}

void delay_s(uint32_t ns)
{
	while(ns--)
	{
		delay_ms(1000);
	}
}
