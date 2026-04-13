#include "timer.h"
#include "tasks.h"

 

//定时器初始化函数
void timer_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 新增：使能TIM2时钟（APB1总线，TIM2属于APB1）	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


	
    TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				// 预分频器：16800（在换算时会自动+1）；84MHz/8400=10KHz
    TIM_TimeBaseStructure.TIM_Period = 10000/50-1;      		// 1000Hz即1ms触发一次  0-10   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	// 二次分频   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 配置TIM9中断通道
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; // TIM9对应的中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断通道
    NVIC_Init(&NVIC_InitStructure);
	
	// 新增：使能TIM2更新中断（溢出中断）
	

	// 新增：使能TIM2计数器
	TIM_Cmd(TIM2, ENABLE);

}




//中断服务函数
void TIM2_IRQHandler(void)
{ 
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		timer_isr();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
    
}



