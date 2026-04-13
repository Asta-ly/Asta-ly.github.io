#ifndef _STEP_H_
#define _STEP_H_
#include "stm32f4xx.h"

extern u16 Servo2_Status;

void Step_Motor_Init(void);

void Step_Motor_One(void);
void Step_Motor_One_U(void);
void Set_Servo2_Angle(uint16_t angle);

#endif