#include "key.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "bit_band.h"
#include <stdio.h>
#include "pwm.h"
#include "led.h"
#include "beep.h"
#include <stdlib.h>
#include <string.h>


uint8_t pass_word[6] = "123456";
uint8_t key_v;
uint8_t error_num;
uint8_t input_word[6] = {0};

void key_4X4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	//输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOE,&GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	PAout(0) = 1;
	PAout(1) = 1;
	PAout(2) = 1;
	PAout(3) = 1;
}


/*
	1 DCMI_VSYNC PB7/TIM4_CH2/U1_RX/FSMC_NL/I2C1_SDA/DCMI_VSYNC       //第一行
	2 DCMI_HREF PA4/SPI1_NSS/U2_CK/DCMI_HSYNC/OTG_HS_SOF/I2S3_WS/DAC1_OUT/ADC12_IN4
	3 DCMI_RESET PB15/U6_CTS/DCMI_D13
	4 DCMI_D1 PC7/TIM3_CH2/TIM8_CH2/U6_RX/SDIO_D7/DCMI_D1/I2S3_MCK

	5 DCMI_D3 PC9/TIM3_CH4/TIM8_CH4/I2C3_SDA/SDIO_D1/DCMI_D3/MCO2/I2S_CKIN  //第一列
	6 DCMI_D5 PB6/TIM4_CH1/U1_TX/CAN2_TX/I2C1_SCL/DCMI_D5
	7 DCMI_D7 PE6/TRACED3/FSMC_A22/TIM9_CH2/DCMI_D7
	8 DCMI__XCLK PA8/TIM1_CH1/U1_CK/I2C3_SCL/MCO1/OTG_FS_SOF
	(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)

*/
uint16_t col,row;
uint8_t key_val = 17; // 初始化为无按键
//行
GPIO_TypeDef* GPIO_row[] = {GPIOA,GPIOA,GPIOA,GPIOA};
uint16_t GPIO_pins_row[] = {GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3};
//列
GPIO_TypeDef* GPIO_col[] = {GPIOA,GPIOA,GPIOA,GPIOA};
uint16_t GPIO_pins_col[] = {GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7};
uint8_t key_Value[4][4] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};

uint8_t button_scan()
{
	for(row = 0;row < 4; row++)
	{
		GPIO_ResetBits(GPIO_row[row],GPIO_pins_row[row]);
		delay_ms(20);
		//printf("%d cow\n",row);
		for(col = 0; col < 4; col++)
		{
			//printf("%d col\n",col);
			if(GPIO_ReadInputDataBit(GPIO_col[col],GPIO_pins_col[col]) == RESET)
			{
				key_val = key_Value[row][col];
				while (GPIO_ReadInputDataBit(GPIO_col[col], GPIO_pins_col[col]) == RESET);
                //delay_ms(20); // 释放后再次消抖
                return key_val;
			}
		}
		GPIO_SetBits(GPIO_row[row],GPIO_pins_row[row]);
	}
	return 17;
}
