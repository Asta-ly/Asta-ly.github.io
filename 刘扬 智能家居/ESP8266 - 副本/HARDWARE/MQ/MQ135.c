#include "MQ135.h"
#include "USART.h"

//ADC1_ch_9  PB1


void MQ135_Init(void)
{
	ADC1_Config();
}


uint16_t MQ135_ADC_Read(void)
{
	//设置指定ADC的规则组通道，采样时间
	return adc_data[1];
}

uint16_t MQ135_GetData(void)
{

	uint32_t  tempData = 0;
	uint8_t i;
	for ( i = 0; i < MQ135_READ_TIMES; i++)
	{
		tempData += MQ135_ADC_Read();
		delay_ms(5);
	}

	tempData /= MQ135_READ_TIMES;
	return tempData;
}


//XIUGAI
float MQ135_GetData_PPM(void)
{
	float  tempData = 0;
	uint8_t i;
	float Vol,RS,R0;
	float ppm;
	for (i = 0; i < MQ135_READ_TIMES; i++)
	{
		tempData += MQ135_ADC_Read();
		//printf("tempdata:%.2f\r\n",tempData);
		delay_ms(5);
	}
	tempData /= MQ135_READ_TIMES;
	
	Vol = (tempData*5/4096);
	RS = (5-Vol)/(Vol*0.5);
	R0=6.64;
	
	ppm = 110.0f*pow(RS/R0,-2.62f);
	
	return ppm;
}

