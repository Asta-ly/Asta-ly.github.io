#include "pwm.h"
#include "stm32f4xx.h"
#include "delay.h"

uint8_t lock_state  = 0;
void PWM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    	/* Enable TIM9 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    	
    	/* Configure PE5 in output pushpull mode：配置GPIO的E端口的5号引脚为推挽复用模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  			// 指定设置GPIO端口的引脚：5号引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// 指定设置GPIO端口的模式：GPIO复用模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 指定设置GPIO端口的输出模式：推挽模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  // 指定设置GPIO端口的速度：高速100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    // 指定设置GPIO端口的上/下拉模式：不使用
    GPIO_Init(GPIOE, &GPIO_InitStructure); // 按照GPIO结构体的初始化配置GPIO端口E
    
    /* Connect TIM9 pins to AF：将GPIOE5复用为定时器功能，将引脚的控制权交由硬件定时器进行控制 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
    
    /* TIM14 configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				// 预分频器：16800（在换算时会自动+1）；84MHz/8400=10KHz
    TIM_TimeBaseStructure.TIM_Period = 10000/50-1;      		// 50Hz即20ms触发一次  0-199   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	// 二次分频   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // PWM模式1 比较值>计数值为 有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较
    TIM_OCInitStructure.TIM_Pulse = 0;	//duty=比较值/计数值*100  // 比较值：79 	计数值0-79时为有效电平			
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出有效电平的极性为高电平
    TIM_OC1Init(TIM9, &TIM_OCInitStructure);
    
    // 使能通道1自动重载：使之周期性的输出PWM
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
    
    // 使能自动重载寄存器
    TIM_ARRPreloadConfig(TIM9, ENABLE);
    
    /* 使能TIM9 */
    TIM_Cmd(TIM9, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare1(TIM9, Compare);
}

uint8_t PWM_GetCompare1(void)
{
	
	return TIM_GetCapture1(TIM9);
}
void PWM_Set(void)
{
	if(lock_state == 0)
		TIM_SetCompare1(TIM9,5);
	else if(lock_state == 1)
		TIM_SetCompare1(TIM9,15);
	delay_ms(50);
}
