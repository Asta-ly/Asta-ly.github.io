#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f4xx.h"


#define BEEP_GPIO_PORT GPIOB
#define BEEP_GPIO_PIN GPIO_Pin_5


#define BEEP_ON 1
#define BEEP_OFF 0

extern uint8_t Beep_Status;

void Beep_Init(void);
void Beep_Set(uint8_t status);

#endif
