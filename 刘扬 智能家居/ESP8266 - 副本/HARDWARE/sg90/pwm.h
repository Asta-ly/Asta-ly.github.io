#ifndef _PWM_H_
#define _PWM_H_
#include "stm32f4xx.h"

extern uint8_t lock_state;
extern uint8_t Led_state;
extern uint8_t Fan_state;
extern uint8_t Fan_Mode;
extern uint8_t Led_Mode;


void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
uint8_t PWM_GetCompare1(void);
uint8_t PWM_GetCompare2(void);
void PWM_Set(void);
void PWM_Set_LED(uint16_t Compare);
void PWM_Set_Fan_compare(uint16_t Compare);
uint8_t PWM_GetCompareFan(void);
void PWM_Set_Fan(uint16_t Compare);\


void Auto_Fan(void);
void Auto_Led(void);
void Set_Fan_Mode(uint8_t Mode);
void Set_Led_Mode(uint8_t Mode);

uint8_t Get_Fan_Mode(void);
uint8_t Get_Led_Mode(void);



#endif
