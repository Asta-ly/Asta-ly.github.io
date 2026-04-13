#include "step.h"
#include "stm32f4xx.h"
#include "bit_band.h"
#include "delay.h"
#include "usart.h"
//
u16 angle;//晾衣架
u16 servo2_angle;//窗帘
u16 Servo2_Status = 0;
/*
	4 DCMI_D1 PC7/TIM3_CH2/TIM8_CH2/U6_RX/SDIO_D7/DCMI_D1/I2S3_MCK
	5 DCMI_D3 PC9/TIM3_CH4/TIM8_CH4/I2C3_SDA/SDIO_D1/DCMI_D3/MCO2/I2S_CKIN  //第一列
	6 DCMI_D5 PB6/TIM4_CH1/U1_TX/CAN2_TX/I2C1_SCL/DCMI_D5
	7 DCMI_D7 PE6/TRACED3/FSMC_A22/TIM9_CH2/DCMI_D7
	(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)

*/
//窗帘
void Step_Motor_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	//输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

/******************************步进电机的驱动************************************* 
;FOSC = 12MHz 
;--------------------------------------------------------------------------------- 
; 步进电机的驱动信号必须为脉冲信号!!! 转动的速度和脉冲的频率成正比!!! 
; 本步进电机步进角为 5.625 度 . 一圈 360 度 , 需要 64 个脉冲完成!!! 
;--------------------------------------------------------------------------------- 
; A 组线圈对应 P0.0 D7  PE6
; B 组线圈对应 P0.1 D5  PB6
; C 组线圈对应 P0.2 D3  PC9
; D 组线圈对应 P0.3 D1  PC7
; 正转次序: AB 组--BC 组--CD 组--DA 组 (即一个脉冲,正转 5.625 度) 
;---------------------------------------------------------------------------------- 
**********************************************************************************/ 

u8 table1[]={0x03,0x06,0x0c,0x09}; /*正转表*/ 
u8 table2[]={0x03,0x09,0x0c,0x06}; /*反转表*/ 

void Step_Motor_One(void)
{
	uint8_t j = 0;
	for(int i = 0; i < 1050; i++)
	{
		
		PEout(6) = (table1[j] & 0x01) >> 0;
		PBout(6) = (table1[j] & 0x02) >> 1;
		PCout(9) = (table1[j] & 0x04) >> 2;
		PCout(7) = (table1[j] & 0x08) >> 3;
		j++;
		if(j > 3)
			j = 0;
		delay_ms(2);
	}
}
void Step_Motor_One_U(void)
{
	uint8_t j = 0;
	for(int i = 0; i < 1050; i++)
	{
		
		PEout(6) = (table2[j] & 0x01) >> 0;
		PBout(6) = (table2[j] & 0x02) >> 1;
		PCout(9) = (table2[j] & 0x04) >> 2;
		PCout(7) = (table2[j] & 0x08) >> 3;
		j++;
		if(j > 3)
			j = 0;
		delay_ms(2);
	}
}

void Set_Servo2_Angle(uint16_t angle)
{
	
	if(angle < 0 || angle > 100)
	{
		printf("angle error\r\n");
		return ;
	}
	if(Servo2_Status < angle)
	{
		for(int i = 0; i < (6*(angle-Servo2_Status))/100; i++)
		{
			Step_Motor_One();
			delay_ms(100);
			printf("%d \r\n",i);
		}
	}
	else
	{
		for(int i = 0; i < (6*(Servo2_Status - angle))/100; i++)
		{
			Step_Motor_One_U();
			delay_ms(100);
			printf("%d \r\n",i);
		}
	
	
	}
	Servo2_Status = angle;

}



