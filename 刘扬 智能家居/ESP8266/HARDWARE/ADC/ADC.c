#include "ADC.h"
#include "usart.h"
#include <stdlib.h> // 引入malloc头文件
#include <string.h> // 引入memset头文件
//PB0,PB1,PA7Z

// 分配3个uint16_t的内存（3×2=6字节）
uint16_t *adc_data = NULL; 



void ADC1_Config(void)
{
	
    // 分配3个uint16_t的内存（4×2=6字节）
    adc_data = (uint16_t *)malloc(4 * sizeof(uint16_t));
    
    // 检查malloc是否分配成功（避免空指针）
    if(adc_data == NULL)
    {
        // 分配失败处理（如卡死、打印错误）
      printf("分配失败");
    }
    
    // 初始化内存为0（避免脏数据）
    memset(adc_data, 0, 4 * sizeof(uint16_t));
	
	
	GPIO_InitTypeDef       GPIO_InitStructure;
	ADC_InitTypeDef        ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStruct;

	ADC_CommonInitTypeDef  ADC_CommonInitStructure;  
	
	/* Enable peripheral clocks *************************************************/
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/* Configure ADC Channel 12 pin as analog input *****************************/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	/* ADC Common configuration *************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;  
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 regular channel 12 configuration ************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 4;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 3, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 4, ADC_SampleTime_480Cycles);
	
	 DMA_DeInit(DMA2_Stream0);
    DMA_InitStruct.DMA_Channel = DMA_Channel_0; // ADC1对应DMA通道0
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC数据寄存器
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)adc_data; // 内存数组
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory; // 外设→内存
    DMA_InitStruct.DMA_BufferSize = 4; // 4个通道
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 16位
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; // 循环模式（持续采集）
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    
    // 7. 使能DMA和ADC
    DMA_Cmd(DMA2_Stream0, ENABLE);
    ADC_DMACmd(ADC1, ENABLE); // ADC触发DMA请求
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); 
		/* 清除转换完成标志 */
	//ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_SoftwareStartConv(ADC1);
 }  


 