#include "BMP280.h"
#include "math.h"
#include "USART.h"
#include "stdio.h"
 
 
unsigned long int hum_raw, temp_raw, pres_raw;
signed long int t_fine;
 
// ???????
static float bme280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/);
void writeReg(uint8_t reg_address, uint8_t data);
void readData(void);
void readTrim(void);
 
// ????????
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
int8_t  dig_H1;
int16_t dig_H2;
int8_t  dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t  dig_H6;
 
// ?????BME280???
void setup()
{
    uint8_t osrs_t = 1;             // ????? x 1
    uint8_t osrs_p = 1;             // ????? x 1
    uint8_t osrs_h = 1;             // ????? x 1
    uint8_t mode = 3;               // ????
    uint8_t t_sb = 5;               // ?????? 1000ms
    uint8_t filter = 0;             // ?????
    uint8_t spi3w_en = 0;           // 3?SPI??
    
    // ?????????????????
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    uint8_t ctrl_hum_reg  = osrs_h;
    
    // ?????
    writeReg(0xF2, ctrl_hum_reg);  // ???????
    writeReg(0xF4, ctrl_meas_reg); // ???????
    writeReg(0xF5, config_reg);    // ?????
    readTrim();                    // ??????
}
 
// ??????????(??????)
void readTrim(void)
{
    uint8_t data[32];
	
    // I2C??????????
    iicDevRead(BME280_ADDRESS, 0x88, 24, &data[0]);
    iicDevRead(BME280_ADDRESS, 0xA1, 1, &data[24]);
    iicDevRead(BME280_ADDRESS, 0xE1, 7, &data[25]);
 
    // ??????
    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11] << 8) | data[10];
    dig_P4 = (data[13] << 8) | data[12];
    dig_P5 = (data[15] << 8) | data[14];
    dig_P6 = (data[17] << 8) | data[16];
    dig_P7 = (data[19] << 8) | data[18];
    dig_P8 = (data[21] << 8) | data[20];
    dig_P9 = (data[23] << 8) | data[22];
    dig_H1 = data[24];
    dig_H2 = (data[26] << 8) | data[25];
    dig_H3 = data[27];
    dig_H4 = (data[28] << 4) | (0x0F & data[29]);
    dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    dig_H6 = data[31];   
}
 
// ?BME280????????
void writeReg(uint8_t reg_address, uint8_t data)
{
    iicDevWriteByte(BME280_ADDRESS, reg_address, data);
}
 
// ??BME280?????
void readData(void)
{
    u8 data[8];
    
    // ????????????
    iicDevRead(BME280_ADDRESS, 0xF7, 8, data);
    pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    hum_raw  = (data[6] << 8) | data[7];
}
 
// ??????
signed long int calibration_T(signed long int adc_T)
{
    signed long int var1, var2, T;
    
    // ????????????????
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1 << 1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T >> 4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}
 
// ??????
unsigned long int calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    
    // ????????????????
    var1 = (((signed long int)t_fine) >> 1) - (signed long int)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1 * ((signed long int)dig_P5)) << 1);
    var2 = (var2 >> 2) + (((signed long int)dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((signed long int)dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((signed long int)dig_P1)) >> 15);
    
    // ?????
    if (var1 == 0)
    {
        return 0;
    }
 
    // ?????????????
    P = (((unsigned long int)(((signed long int)1048576) - adc_P) - (var2 >> 12))) * 3125;
    
    if (P < 0x80000000)
    {
        P = (P << 1) / ((unsigned long int)var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P >> 3) * (P >> 3)) >> 13))) >> 12;
    var2 = (((signed long int)(P >> 2)) * ((signed long int)dig_P8)) >> 13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    
    return P;
}
 
// ??????
unsigned long int calibration_H(signed long int adc_H)
{
    signed long int v_x1;
    
    // ?????????????????
    v_x1 = (t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) - (((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + ((signed long int)16384)) >> 15) *
            (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int)32768))) >> 10) + ((signed long int)2097152)) *
            ((signed long int)dig_H2) + 8192) >> 14));
            
    v_x1 = v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4);
    if (v_x1 < 0) v_x1 = 0;
    if (v_x1 > 419430400) v_x1 = 419430400;
    return (unsigned long int)(v_x1 >> 12);
}
 
void bme280Init(void)
{	
	IIC_Init();		                                                           /*???I2C*/
	delay_ms(20);
	setup();
}
 
void bme280GetData(float* pressure,float* temperature,float* asl)
{
    double temp_act = 0.0, press_act = 0.0;
    signed long int temp_cal;
    unsigned long int press_cal;
    readData();
	
    temp_cal = calibration_T(temp_raw);
    press_cal = calibration_P(pres_raw);
    temp_act = (double)temp_cal / 100.0;
    press_act = (double)press_cal / 100.0;

 
	*temperature=temp_act;                                                     /*???*/
	*pressure=press_act;	                                                   /*??hPa*/	
	*asl=bme280PressureToAltitude(pressure);	                               /*?????*/	
}
 
		                                              
/*
 * Converts pressure to altitude above sea level (ASL) in meters
*/
static float bme280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/)
{
    if (*pressure>0)
    {
        return((pow((1015.7f/ *pressure),CONST_PF)-1.0f)*(FIX_TEMP+273.15f))/0.0065f;
    }
    else
    {
        return 0;
    }
}
 
//???IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(SCL_GPIO_CLK, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin=SCL_GPIO_PIN|SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;	//????
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(SCL_GPIO_PORT,&GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;
 
}
 
// ????I/O??????
static void SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SDA_GPIO_CLK, ENABLE);
 
	GPIO_InitStructure.GPIO_Pin = SDA_GPIO_PIN; // PC.10  DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;	//????
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SDA_GPIO_PORT, &GPIO_InitStructure); // ???GPIOC.10
}
// ????I/O??????
static void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SDA_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SDA_GPIO_PIN; // PC.10 DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(SDA_GPIO_PORT, &GPIO_InitStructure); // ???GPIOC.10
}
 
 
//??IIC????
void IIC_Start(void)
{
	SDA_OUT();                                                                 //sda???
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;                                                                 //START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;                                                                 //??I2C??,????????? 
}	  
//??IIC????
void IIC_Stop(void)
{
	SDA_OUT();                                                                 //sda???
	IIC_SCL=0;
	IIC_SDA=0;                                                                 //STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;                                                                 //??I2C??????
	delay_us(4);							   	
}
//????????
//???:1,??????
//        0,??????
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();                                                                  //SDA?????  
	IIC_SDA=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;                                                                 //????0 	   
	return 0;  
} 
//??ACK??
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//???ACK??		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC??????
//????????
//1,???
//0,???			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;                                                                 //??????????
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);                                                           //?TEA5767??????????
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//?1???,ack=1?,??ACK,ack=0,??nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();                                                                  //SDA?????
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();                                                            //??nACK
    else
        IIC_Ack();                                                             //??ACK   
    return receive;
}
 
//???????????
//ReadAddr:???????  
//???  :?????
u8 iicDevReadByte(u8 devaddr,u8 addr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(devaddr);                                                    //??????? 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(addr);                                                       //?????
	IIC_Wait_Ack();	
 
	IIC_Start();  	 	   
	IIC_Send_Byte(devaddr|1);                                                  //???????			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);			   
	IIC_Stop();                                                                //????????	    
	return temp;
}
 
//???????
//addr:????
//rbuf:?????
//len:????
void iicDevRead(u8 devaddr,u8 addr,u8 len,u8 *rbuf)
{
	int i=0;
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(addr);                                                       //????  
	IIC_Wait_Ack();	
 
	IIC_Start();  	
	IIC_Send_Byte(devaddr|1);  	
	IIC_Wait_Ack();	
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //?????????
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );	
}
 
 
void iicDevReadCal1(u8 devaddr,u8 addr,u8 len,u8 *rbuf)
{
	int i=0;
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0x88);                                                       //????  
	IIC_Wait_Ack();	
	len=24;
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //?????????
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );	
	delay_us(100);
	
	IIC_Start();  
	IIC_Send_Byte(0x76);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0xA1);                                                       //????  
	IIC_Wait_Ack();	
	rbuf[i++]=IIC_Read_Byte(0); 
	IIC_Stop( );
	delay_us(100);
	
	IIC_Start();  
	IIC_Send_Byte(0x76);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0xE1);                                                       //????  
	IIC_Wait_Ack();	
	len=32;
	for(; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //?????????
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );	
}
 
//???????
//addr:????
//rbuf:?????
//len:????
void iicDevReadCal(u8 devaddr,u8 addr,u8 len,bme280Calib *bme280Ctype)
{
	int i=0;
	u8 rbuf[32];
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0x88);                                                       //????  
	IIC_Wait_Ack();	
	len=24;
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //?????????
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );	
	delay_us(100);
	
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0xA1);                                                       //????  
	IIC_Wait_Ack();	
	rbuf[i++]=IIC_Read_Byte(0); 
	IIC_Stop( );
	delay_us(100);
	
	IIC_Start();  
	IIC_Send_Byte(devaddr);  
	IIC_Wait_Ack();	
	IIC_Send_Byte(0xE1);                                                       //????  
	IIC_Wait_Ack();	
	len=32;
	for(; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=IIC_Read_Byte(0);                                          //?????????
		}
		else
			rbuf[i]=IIC_Read_Byte(1);
	}
	IIC_Stop( );
	
	bme280Ctype->dig_T1=(rbuf[1] << 8) | rbuf[0];
	bme280Ctype->dig_T2=(rbuf[3] << 8) | rbuf[2];
	bme280Ctype->dig_T3=(rbuf[5] << 8) | rbuf[4];
	bme280Ctype->dig_P1=(rbuf[7] << 8) | rbuf[6];
	bme280Ctype->dig_P2=(rbuf[9] << 8) | rbuf[8];
	bme280Ctype->dig_P3=(rbuf[11]<< 8) | rbuf[10];
	bme280Ctype->dig_P4=(rbuf[13]<< 8) | rbuf[12];
	bme280Ctype->dig_P5=(rbuf[15]<< 8) | rbuf[14];
	bme280Ctype->dig_P6=(rbuf[17]<< 8) | rbuf[16];
	bme280Ctype->dig_P7=(rbuf[19]<< 8) | rbuf[18];
	bme280Ctype->dig_P8=(rbuf[21]<< 8) | rbuf[20];
	bme280Ctype->dig_P9=(rbuf[23]<< 8) | rbuf[22];
	bme280Ctype->dig_H1=rbuf[24];
	bme280Ctype->dig_H2=(rbuf[26]<< 8) | rbuf[25];
	bme280Ctype->dig_H3=rbuf[27];
	bme280Ctype->dig_H4=(rbuf[28]<< 4) | (0x0F & rbuf[29]);
	bme280Ctype->dig_H5=(rbuf[30] << 4) | ((rbuf[29] >> 4) & 0x0F);
	bme280Ctype->dig_H6=rbuf[31];  
}
 
//???????????
//WriteAddr :?????????    
//DataToWrite:??????
void iicDevWriteByte(u8 devaddr,u8 addr,u8 data)
{				   	  	    																 
	IIC_Start();  
	IIC_Send_Byte(devaddr);                                                    //??????? 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(addr);                                                       //?????
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(data);                                                       //????							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();		                                                           //???????? 	 
}
 
//???????
//addr:????
//wbuf:?????
//len:?????
void iicDevWrite(u8 devaddr,u8 addr,u8 len,u8 *wbuf)
{
	int i=0;
	IIC_Start();  
	IIC_Send_Byte(devaddr);  	
	IIC_Wait_Ack();	
	IIC_Send_Byte(addr);  //????
	IIC_Wait_Ack();	
	for(i=0; i<len; i++)
	{
		IIC_Send_Byte(wbuf[i]);  
		IIC_Wait_Ack();		
	}
	IIC_Stop( );	
}


