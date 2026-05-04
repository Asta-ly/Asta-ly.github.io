#include "pwm.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "bit_band.h"
#include "SU_03T.h"
#include "dht11.h"
#include "LIGHT.h"


uint8_t lock_state  = 0;
uint8_t Led_state  = 0; 
uint8_t Fan_state  = 0; 
uint8_t Fan_Mode = 0;
uint8_t Led_Mode = 0;

extern u8 humidityH;	  //湿度整数部分
extern u8 humidityL;	  //湿度小数部分
extern u8 temperatureH;   //温度整数部分
extern u8 temperatureL;   //温度小数部分

void PWM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    	/* Enable TIM9 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    	
    	/* Configure PE5 in output pushpull mode：配置GPIO的E端口的5号引脚为推挽复用模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_9; 			// 指定设置GPIO端口的引脚：5号引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// 指定设置GPIO端口的模式：GPIO复用模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 指定设置GPIO端口的输出模式：推挽模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  // 指定设置GPIO端口的速度：高速100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    // 指定设置GPIO端口的上/下拉模式：不使用
    GPIO_Init(GPIOE, &GPIO_InitStructure); // 按照GPIO结构体的初始化配置GPIO端口E
    
    /* Connect TIM9 pins to AF：将GPIOE5复用为定时器功能，将引脚的控制权交由硬件定时器进行控制 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
    
    /* TIM14 configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				// 预分频器：16800（在换算时会自动+1）；84MHz/8400=10KHz
    TIM_TimeBaseStructure.TIM_Period = 10000/50-1;      		// 50Hz即20ms触发一次  0-199   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	// 二次分频   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				// 预分频器：16800（在换算时会自动+1）；84MHz/8400=10KHz
    TIM_TimeBaseStructure.TIM_Period = 10000/50-1;      		// 50Hz即20ms触发一次  0-199   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	// 二次分频   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // PWM模式1 比较值>计数值为 有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较
    TIM_OCInitStructure.TIM_Pulse = 0;	//duty=比较值/计数值*100  // 比较值：79 	计数值0-79时为有效电平			
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出有效电平的极性为高电平
    TIM_OC1Init(TIM9, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // PWM模式1 比较值>计数值为 有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较
    TIM_OCInitStructure.TIM_Pulse = 0;	//duty=比较值/计数值*100  // 比较值：79 	计数值0-79时为有效电平			
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出有效电平的极性为高电平
    TIM_OC2Init(TIM9, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // PWM模式1 比较值>计数值为 有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较
    TIM_OCInitStructure.TIM_Pulse = 0;	//duty=比较值/计数值*100  // 比较值：79 	计数值0-79时为有效电平			
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出有效电平的极性为高电平
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    
    // 使能通道1自动重载：使之周期性的输出PWM
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    
    // 使能自动重载寄存器
    TIM_ARRPreloadConfig(TIM9, ENABLE);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    
    /* 使能TIM9 */
    TIM_Cmd(TIM9, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare1(TIM9, Compare);
}
void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare2(TIM9, Compare);
}
uint8_t PWM_GetCompare2(void)
{
    return TIM_GetCapture2(TIM9);
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
void PWM_Set_LED(uint16_t Compare)
{
    if(Led_state == 0)
    {
		TIM_SetCompare2(TIM9,0);
    }
    else
    {
        TIM_SetCompare2(TIM9,Compare);
    }
    delay_ms(50);
}
void PWM_Set_Fan_compare(uint16_t Compare)
{
    TIM_SetCompare1(TIM1, Compare);
}
uint8_t PWM_GetCompareFan(void)
{
    return TIM_GetCapture1(TIM1);
}
void PWM_Set_Fan(uint16_t Compare)    
{
     if(Fan_state == 0)
    {
		TIM_SetCompare1(TIM1,0);
    }
    else
    {
        TIM_SetCompare1(TIM1,Compare);
    }
}
// 测试通道2

void Auto_Led(void)
{
    if(Led_Mode == 0)
    {
        return ;
    }
    if(LIGHT_GetIntensity_lx() > 1000)
    {
        PWM_Set_LED(200);
    }
    else if(LIGHT_GetIntensity_lx() < 1000 && LIGHT_GetIntensity_lx() > 500)
    {
        PWM_Set_LED(100);
    }
    else
    {
        PWM_Set_LED(0);
    }
}
void Auto_Fan(void)
{
    if(Fan_Mode == 0)
    {
        return ;
    }
    if(temperatureH + temperatureL > 30)
    {
        PWM_Set_Fan(200);
    }
    else
    {
        PWM_Set_Fan(0);
    }
}
void Set_Fan_Mode(uint8_t Mode)
{
    Fan_Mode = Mode;
}
void Set_Led_Mode(uint8_t Mode)
{
    Led_Mode = Mode;
}
uint8_t Get_Fan_Mode(void)
{
    return Fan_Mode;
}
uint8_t Get_Led_Mode(void)
{
    return Led_Mode;
}



