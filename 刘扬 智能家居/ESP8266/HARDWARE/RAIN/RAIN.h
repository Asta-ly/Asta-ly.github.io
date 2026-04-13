#ifndef __RAIN_H
#define	__RAIN_H
#include "stm32f4xx.h"
#include "ADC.h"
#include "delay.h"
#include "math.h"


#define RAIN_READ_TIMES	10  


void RAIN_Init(void);
uint16_t RAIN_GetData(void);

#endif /* __RAIN_H */

