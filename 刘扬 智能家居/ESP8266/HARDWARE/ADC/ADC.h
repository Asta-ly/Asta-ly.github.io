#ifndef _ADC_H_
#define _ADC_H_
#include "stm32f4xx.h"
#include "delay.h"
#include "bit_band.h"

extern uint16_t *adc_data;
void ADC1_Config(void);

#endif
