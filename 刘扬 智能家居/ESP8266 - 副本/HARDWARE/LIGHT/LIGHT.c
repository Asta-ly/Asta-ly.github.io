#include "LIGHT.h"
#include "USART.h"

//ADC1_ch_11 PC1

void LIGHT_Init(void)
{
	ADC1_Config();
}


uint16_t LIGHT_ADC_Read(void)
{
	//设置指定ADC的规则组通道，采样时间
	printf("%d\r\n",adc_data[2]);
	return adc_data[2];
}

float LIGHT_GetData(void)
{

	uint32_t  tempData = 0;
	uint8_t i;
	for ( i = 0; i < LIGHT_READ_TIMES; i++)
	{
		tempData += LIGHT_ADC_Read();
		delay_ms(5);
	}

	float avg_adc = (float)tempData/LIGHT_READ_TIMES;
	float voltage= (avg_adc*ADC_REF_VOLTAGE/ADC_MAX_VALUE);
	
	//  printf("Avg_ADC:%.0f → Voltage:%.3f V\r\n", avg_adc, voltage);
	return voltage;
}


float LIGHT_GetIntensity_lx(void)
{
    float voltage = LIGHT_GetData();
    
    // 光敏电阻电压→光照强度转换公式（需根据硬件校准）
    // 公式说明：光敏电阻阻值随光照增强而减小，电压也减小，需实际校准
    float lx = 10000.0f * pow((ADC_REF_VOLTAGE - voltage)/voltage, 1.2f);
    
    // 范围限制（避免异常值）
    if(lx < 0.0f) lx = 0.0f;
    if(lx > 10000.0f) lx = 10000.0f;
    
    return lx;
}