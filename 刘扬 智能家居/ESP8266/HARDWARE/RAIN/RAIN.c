#include "RAIN.h"
#include "USART.h"

//ADC1_ch_12  PC2

#include "rain.h"
void RAIN_Init(void)
{
	ADC1_Config();
}

uint16_t RAIN_ADC_Read(void)
{
	printf("RAIN:%d\r\n",adc_data[3]);
	return adc_data[3];
}


uint16_t RAIN_GetData(void)
{
	

	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < RAIN_READ_TIMES; i++)
	{
		tempData += RAIN_ADC_Read();
		delay_ms(5);
	}

	tempData /= RAIN_READ_TIMES;
	return 4095-tempData;
}