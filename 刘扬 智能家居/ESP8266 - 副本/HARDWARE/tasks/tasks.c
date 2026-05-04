#include "tasks.h"
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
#include "step.h"
#include <stdlib.h> // 引入malloc头文件
#include "SU_03T.h"


uint32_t task1_cnt = 0;
uint32_t task2_cnt = 0;
uint32_t task3_cnt = 0;

uint8_t task1_flag = 0;
uint8_t task2_flag = 0;
uint8_t task3_flag = 0;

uint16_t adc_value[2] = {0};
uint8_t led_status = 0, voice_status_new = 0, voice_status_old = 0;

//dht11添加变量
extern u8 humidityH;	  //湿度整数部分
extern u8 humidityL;	  //湿度小数部分
extern u8 temperatureH;   //温度整数部分
extern u8 temperatureL;   //温度小数部分
extern DHT11_Data_TypeDef DHT11_data;
extern u8  flag;
 //灯的状态
 // u8 LED0_FLAG;
 // u8 LED1_FLAG;
 
extern u16 light,rain,Pre;//光照，雨滴，大气压强
extern float ppm,aqi,CO2;//烟雾浓度，空气质量，二氧化碳浓度
extern float bmp280_press,bmp280;
unsigned char *dataPtr = NULL;


void timer_isr(void)
{
    if (task1_cnt < 2)
        task1_cnt++;
    else
    {
        task1_flag = 1;
        task1_cnt = 0;
    }

    if (task2_cnt < 20)
        task2_cnt++;
    else
    {
        task2_flag = 1;
        task2_cnt = 0;
    }
    
    if (task3_cnt < 1)
        task3_cnt++;
    else
    {
        task3_flag = 1;
        task3_cnt = 0;
    }
    
    //Get_Value();
    Update_Value();
	Get_Onenet_Cmd();
	SU_03T_Cmd();
	//Auto_Fan();
	//Auto_Led();
//    printf("value1:%.2f, value2:%.2f\r\n", (float)adc_value[0]/4096*3.3, (float)adc_value[1]/4096*3.3);
}

void Get_Value(void)
{
    if(task1_flag == 0)
        return;
	
	
	if(PWM_GetCompare1() == 5)
			lock_state = 0;
	else
			lock_state = 1;
    if(Read_DHT11(&DHT11_data))
		{
			humidityH = DHT11_data.humi_int;	  //湿度整数部分
			humidityL = DHT11_data.humi_deci;	  //湿度小数部分
			temperatureH = DHT11_data.temp_int;   //温度整数部分
			temperatureL = DHT11_data.temp_deci;   //温度小数部分
		}
		
		ppm = MQ2_GetData_PPM();
//		printf("ppm %.2f\r\n",ppm);
		light = LIGHT_GetIntensity_lx();
//		printf( "light %d\r\n",light);
		aqi = MQ135_GetData();
//		printf("AQI %.2f\r\n",aqi);
		CO2 = MQ135_GetData_PPM();
//		printf( "CO2 %.2f\r\n",CO2);
		rain = RAIN_GetData(); 
//		printf("rain %2d\r\n",rain);
		bme280GetData(&bmp280_press,&bmp280,&bmp280);
		Pre = bmp280_press;
//		printf("pressure %2dhpa\r\n",Pre);
    task1_flag = 0;
}

void Update_Value(void)
{ 
	if(task2_flag == 0)
        return;
	printf( "OneNet_SendData\r\n");//通过串口1发送提示信息（要开始发送数据了）
	OneNet_SendData();//发送数据给onenet
	printf("send_data_end\n");
	ESP8266_Clear();
	task2_flag = 0;
}

void Get_Onenet_Cmd(void)
{
	if(task3_flag == 0)
        return;

		dataPtr = ESP8266_GetIPD(0);//获取平台返回的数据
		if(dataPtr != NULL)//如果返回数据不为空
		{
			OneNet_RevPro(dataPtr);//平台返回数据检测
			ESP8266_Clear();
		}

	task3_flag = 0;
}
