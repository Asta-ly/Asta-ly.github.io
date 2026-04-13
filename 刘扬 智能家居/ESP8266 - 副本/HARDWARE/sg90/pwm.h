#ifndef _PWM_H_
#define _PWM_H_
#include "stm32f4xx.h"

extern uint8_t lock_state;

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
uint8_t PWM_GetCompare1(void);
void PWM_Set(void);

#endif
