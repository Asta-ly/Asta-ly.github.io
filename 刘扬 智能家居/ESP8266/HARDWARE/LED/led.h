#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"
#include "bit_band.h"

#define LED0 PFout(9) 
#define LED1 PFout(10)	
void led_init(void);


#endif 
