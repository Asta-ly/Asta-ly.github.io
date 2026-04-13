#include "delay.h"

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 		// Disable SysTick  关闭系统定时器
	SysTick->LOAD = SystemCoreClock/1000000*n-1; 	// 就是nus
	SysTick->VAL = 0; 		// Clear current value as well as count flag，清空当前计数值且清零count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock，启用SysTick定时器且使用处理器时钟 168MHz
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set，等待计数值到达0 --》 置位 Control and Status Register的位16
	SysTick->CTRL = 0; 		// Disable SysTick 关闭系统定时器
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 		// Disable SysTick  关闭系统定时器
		SysTick->LOAD = SystemCoreClock/1000-1; 	// 就是nus
		SysTick->VAL = 0; 		// Clear current value as well as count flag，清空当前计数值且清零count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock，启用SysTick定时器且使用处理器时钟 168MHz
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set，等待计数值到达0 --》 置位 Control and Status Register的位16
	}
	SysTick->CTRL = 0; 		// Disable SysTick 关闭系统定时器

}
