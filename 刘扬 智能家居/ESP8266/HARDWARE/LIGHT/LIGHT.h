#ifndef _LIGHT_H
#define _LIGHT_H


#include "ADC.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "math.h"

#define LIGHT_READ_TIMES	10  //MQ-135传感器ADC循环读取次数
#define ADC_REF_VOLTAGE 3.3f   // F407 ADC参考电压（V）
#define ADC_MAX_VALUE 4095.0f  // 12位ADC最大值

void LIGHT_Init(void);
float LIGHT_GetData(void);
float LIGHT_GetIntensity_lx(void);


#endif
