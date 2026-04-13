#include "MQ2.h"

//ADC1_ch_8  PB0
void MQ2_Init(void)
{
	ADC1_Config();
}

/*************************************************************
*	函数名称:	MQ2_ADC_Read
*************************************************************/

uint16_t MQ2_ADC_Read(void)
{
	//设置指定ADC的规则组通道，采样时间
	//return ADC_GetValue(MQ2_ADC_CHANNEL, ADC_SampleTime_55Cycles5);
	return adc_data[0];
}


uint16_t MQ2_GetData(void)
{
	u8 i;

	uint32_t  tempData = 0;
	for ( i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		delay_ms(5);
	}

	tempData /= MQ2_READ_TIMES;
	return tempData;
}


float MQ2_GetData_PPM(void)
{
	float  tempData = 0;
	
	u8 i;
	for ( i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		delay_ms(5);
	}
	tempData /= MQ2_READ_TIMES;
	
	float Vol = (tempData*3.3/4096);
	float RS = (3.3-Vol)/(Vol*0.5);
	float R0=6.64;
	
	float ppm = 1000.0f*pow(RS/R0, -2.95f);

	return ppm;
}

