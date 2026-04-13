#include "dht11.h"




//#include "DHT11.h"
//#include "delay.h"
//#include "bit_band.h"
//#include "pwm.h"


//uint8_t retry = 0;
//uint8_t byte = 0;
//uint8_t buf[5] = {0};

//u8 temp,humi;
//int error;
//uint8_t temp_max = 30,humi_max = 70,humi_min = 10;

//void DHT11_Pin_Init(GPIOMode_TypeDef GPIO_Mode)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;  // 定义的变量写在函数体的开头
//	
//    /* GPIOF and GPIOE Peripheral clock enable 使能外设时钟，打开端口F和端口E */
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

//	/* Configure PF9 and PF10 in output pushpull mode */
//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;         // 指定配置的引脚，9、10号引脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;   // 指定选择的引脚的工作模式，输出模式
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 指定选择的引脚的速度，高速 
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 指定选择的引脚的上/下拉，不使用
//	
//	if(GPIO_Mode == GPIO_Mode_OUT)
//	{
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	}
//	
//	GPIO_Init(GPIOE, &GPIO_InitStructure); // 根据GPIO_InitTypeDef中指定的参数初始化GPIO端口的外围设备。
//}

//uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
//{
//	DHT11_Pin_Init(GPIO_Mode_OUT);
//	PGout(9) = 0;
//	delay_ms(18);
//	PGout(9) = 1;
//	delay_us(30);
//	
//	DHT11_Pin_Init(GPIO_Mode_IN);
//	
//	//等待响应信号
//	while(PGin(9) == 1)
//	{
//		retry++;
//		delay_us(1);
//		
//		if(retry >= 100) 
//			return -1;
//	}
//	retry = 0;
//	
//	//响应信号
//	while(PGin(9) == 0)
//	{
//		retry++;
//		delay_us(1);
//		
//		
//		if(retry > 81) 
//			return -2;
//	}
//	retry = 0;
//	
//	//间隙信号前的高电平信号
//	while(PGin(9) == 1)
//	{
//		retry++;
//		delay_us(1);
//		
//		
//		if(retry > 100) 
//			return -3;
//	}
//	retry = 0;
//	
//	//开始接收数据
//	for(int j = 0; j < 5; j++)
//	{
//		for(int i = 0; i < 8; i++)
//		{
//			//间隙信号
//			while(PGin(9) == 0)
//			{
//				retry++;
//				delay_us(1);
//				
//				if(retry > 50) 
//					return -4;
//			}
//			retry = 0;
//			//数据信号
//			while(PGin(9) == 1)
//			{
//				retry++;
//				delay_us(1);
//				if(retry > 100)
//					return -4;
//			}
//			if(retry > 30)
//			{
//				buf[j] |= (1 << (7-i));
//			}
//			else
//			{
//				buf[j] &= ~(1 << (7-i));
//			}
//			retry = 0;
//		}
//	}
//	if(buf[4] == buf[0]+ buf[1]+ buf[2]+ buf[3])
//	{
//			DHT11_Data->humi_int=buf[0];
//			DHT11_Data->humi_deci=buf[1];
//			DHT11_Data->temp_int=buf[2];
//			DHT11_Data->temp_deci=buf[3];
//			DHT11_Data->check_sum=buf[4];
//		
//		return 0;
//	}
//	return -5;
//}






void dht11_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; 	//9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;	//输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	//开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;//高速，速度越高，响应越快，但是功耗会更高
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

void dht11_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; 	//9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;	//输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	//开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;//高速，速度越高，响应越快，但是功耗会更高
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}


//复位DHT11
void DHT11_Rst(void)	   
{                 
	dht11_OUT(); 	    //SET OUTPUT
    DHT11_DQ_OUT=0; 	//拉低DQ
    delay_ms(20);    	//拉低至少18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	dht11_IN();//SET INPUT	 
	//start.等待低电平的到来
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	} 
	if(retry>=100)return 1;
	//低电平
	else retry=0;
	
	//在低电平等待高电平的到来
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	}
	
	if(retry>=100)return 1;	
	//高电平
	return 0;
}
//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	//等待低电平
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	//下降沿
	
	//等待数据
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	//数据到来
	delay_us(40);//等待40us
	//数据高电平超过30s为高电平
	if(DHT11_DQ_IN)return 1;
	//低于30s为低电平
	else return 0;		   
}
//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
u8 DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	//打开端口G的硬件时钟，就是供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);


	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; 	//9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	//开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;//高速，速度越高，响应越快，但是功耗会更高
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);			 //PG11 输出高
			    
	DHT11_Rst();  //复位DHT11
	return DHT11_Check();//等待DHT11的回应
} 

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			DHT11_Data->humi_int=buf[0];
			DHT11_Data->humi_deci=buf[1];
			DHT11_Data->temp_int=buf[2];
			DHT11_Data->temp_deci=buf[3];
			DHT11_Data->check_sum=buf[4];
		}
	}else return 1;
	 /*检查读取的数据是否正确*/
	if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
		 return SUCCESS;
	else 
		return ERROR;	    
}

	  


/*************************************END OF FILE******************************/
