#ifndef	_SU_03T_H
#define	_SU_03T_H

#include "stm32f4xx.h"
#include "bit_band.h"

extern uint8_t cmd;
#define Read_PE10 PEin(10)
#define Read_PE11 PEin(11)
#define Read_PE12 PEin(12)
#define Read_PE13 PEin(13)
#define Write_PE9  PEout(9)

void SU_03T_Init(void);
void SU_03T_Cmd();


#endif