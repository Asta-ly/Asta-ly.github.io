#ifndef _MQ135_H_
#define _MQ135_H_

#include "ADC.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "math.h"

#define MQ135_READ_TIMES	10  //MQ-135传感器ADC循环读取次数


void MQ135_Init(void);
float MQ135_GetData_PPM(void);
uint16_t MQ135_GetData(void);

#endif
