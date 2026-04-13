#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f4xx.h"


 void key_init(void);
 uint8_t key_scan(void);
 void key_4X4_Init(void);
 uint8_t button_scan(void);


extern uint8_t pass_word[6];
extern uint8_t key_v;
extern uint8_t error_num;
extern uint8_t i;
extern uint8_t input_word[6];
#endif
