#include "led.h"
#include "delay.h"
#include "bit_band.h"
#include "usart.h"
#include "esp8266.h"
#include "onenet.h"
#include "esp8266.h"
#include "string.h"
#include "pwm.h"
#include "dht11.h"
#include "ADC.h"
#include "BMP280.h"
#include "LIGHT.h"
#include "MQ2.h"
#include "MQ135.h"
#include "RAIN.h"
#include "MFRC522.h"
#include "oled.h"
#include "timer.h"
#include "password.h"
#include "beep.h"
#include "w25q128.h"
#include "tasks.h"
#include "KEY.h"
#include "step.h"
#include "SU_03T.h"


#include <stdlib.h> // 引入malloc头文件

void Hardware_Init(void);
void MFRC522Test(void);
//dht11添加变量
u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分
DHT11_Data_TypeDef DHT11_data;
extern u8  flag;
uint8_t key_last = 0;
//灯的状态
// u8 LED0_FLAG;
// u8 LED1_FLAG;

u16 light,rain,Pre;//光照，雨滴，大气压强
float ppm,aqi,CO2;//烟雾浓度，空气质量，二氧化碳浓度
float bmp280_press,bmp280;
	void door(void);

 int main(void)
 {	
	unsigned short timeCount = 0;	//发送间隔变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Hardware_Init();
	bme280Init();

	PWM_SetCompare1(5);
	delay_ms(10);
	OLED_ShowString(10,1,"ESP8266_INIT...",16);
	printf("ESP8266_INIT...\n");
	ESP8266_Init();	//初始化ESP8266

	OLED_Clear();
	printf("8266_INIT_END\n");
	OLED_ShowString(10,1,"ESP8266_INIT_END",16);
	//LED1=1;
	//LED0=1;
	OLED_ShowString(10,1,"Link onenet...",16);
	 OLED_Clear();
	while(OneNet_DevLink())			//接入OneNET
	delay_ms(500);
	printf("接入onenet成功");
	OLED_ShowString(10,1,"Link onenet AC",16);
	OneNET_Subscribe();
	timer_init();

	 
	while(1) 
	{		
		key_last = button_scan();
		if(key_last == 'A')
		{
			door();
		}
		timer_isr();
	}
	free(adc_data);
}
 
void door(void)
{
	while(1)
	{
		password_check();
		oled_show_input();
		key_last = password_get_input();
		if(key_last == POUND_KEY)
		{
			if(password_compare() == TRUE)
			{
				password_input_right_action();
				
				return ;
			}
			else
				password_input_wrong_action();
		}
		else if(key_last == STAR_KEY)
		{
			oled_show_old();
			password_get_input();
			if(password_compare() == TRUE)
				password_old_right_action();
			else
				password_old_wrong_action();
		}
	}
}


void Hardware_Init(void)
{
	led_init();			     //LED端口初始化
	//DHT11_Init();
	PWM_Init();
	uart_init(115200);//串口1初始化
	uart3_init(115200);//串口3初始化
	
	
	//ADC1_Config();
	IIC_Init();
	ADC1_Config();
	OLED_Init();
	OLED_Clear();
	Beep_Init();
	// W25Qxx_Init();
	key_4X4_Init();
	LIGHT_Init();
	MQ2_Init();
	MQ135_Init();
	RAIN_Init();
	STM32_SPI3_Init();
	//Step_Motor_Init();
	SU_03T_Init();

	printf(" Hardware init OK\r\n");
}


























