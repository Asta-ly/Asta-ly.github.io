#ifndef __PASSWORD_H__
#define __PASSWORD_H__

#include "stm32f4xx.h"

#define POUND_KEY   '#'
#define STAR_KEY    '*'

#define TRUE    1
#define FALSE   0

void password_init(void);
uint8_t password_get_input(void);
uint8_t password_compare(void);
void password_input_right_action(void);
void password_input_wrong_action(void);
void password_old_right_action(void);
void password_old_wrong_action(void);
void password_check(void);

#endif

