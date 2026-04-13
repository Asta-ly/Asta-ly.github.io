#include "beep.h"

uint8_t Beep_Status;
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);
    
    Beep_Set(BEEP_OFF); 
}
void Beep_Set(uint8_t status)
{
	
	GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN, status == BEEP_ON ? Bit_SET : Bit_RESET);		//如果status等于LED_ON，则返回Bit_SET，否则返回Bit_RESET
	
	Beep_Status = status;

}
