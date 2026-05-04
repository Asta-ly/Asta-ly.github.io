#include "step.h"
#include "delay.h"
#include "usart.h"
#include "SU_03T.h"
#include "pwm.h"

uint8_t cmd;

void SU_03T_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	//���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	PEout(8) = RESET;
}
void SU_03T_Cmd()
{
	
	cmd = (Read_PE10 << 3)|(Read_PE11 << 2)|(Read_PE12 << 1)|Read_PE13;

	//printf("命令：%d\n",cmd);
	//printf("PE10：%d,PE11：%d,PE12：%d,PE13：%d\n",Read_PE10,Read_PE11,Read_PE12,Read_PE13);
	//printf("PE9：%d\n",GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_9));
	switch(cmd)
	{
		printf("1\n");
		case 1: Led_state = 1; PWM_Set_LED(100);
				//printf("LED%d\n",PWM_GetCompare2());
				break;
		case 2: Led_state = 0,PWM_Set_LED(0);
				//printf("LED%d\n",PWM_GetCompare2()); break;
		case 3: 
			{
				PWM_Set_LED(30); 
				//printf("LED%d\n",PWM_GetCompare2()); 
				break;
			}
		case 4: 
			{
				PWM_Set_LED(200); 
				printf("LED%d\n",PWM_GetCompare2()); break;
			}
		case 5: Set_Servo2_Angle(100);break;
		case 6: Set_Servo2_Angle(0);break;
		case 7: Set_Servo2_Angle(50);break;
		case 8:	 Fan_state = 1; PWM_Set_Fan(100);
				//printf("Fan%d\n",PWM_GetCompareFan());
				break;
		case 9:  Fan_state = 0; PWM_Set_Fan(0);
				//printf("Fan%d\n",PWM_GetCompareFan());
				break;
		case 10: PWM_Set_Fan(50);
				//printf("Fan%d\n",PWM_GetCompareFan());
				break;
		case 11: PWM_Set_Fan(200);
				//printf("Fan%d\n",PWM_GetCompareFan());
				break;
		case 12: Set_Led_Mode(1);break;
		case 13: Set_Led_Mode(0);break;
		case 14: Set_Fan_Mode(1);break;
		case 15: Set_Fan_Mode(0);break;

	}
	
	//if(Read_PE10 == RESET && Read_PE11 == )

}

