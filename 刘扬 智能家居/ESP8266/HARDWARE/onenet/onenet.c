#include "stm32f4xx.h"
#include "esp8266.h"
#include "onenet.h"
#include "mqttkit.h"
#include "usart.h"
#include "delay.h"
#include "pwm.h"
#include <string.h>
#include <stdio.h>
#include "step.h"

//json库
#include"cjson.h"
#include"led.h"
#define PROID		"k0085M9O94"//产品ID

#define AUTH_INFO	"version=2018-10-31&res=products%2Fk0085M9O94%2Fdevices%2Fuser1&et=2017881776&method=md5&sign=GMmynCS7MYOFPZLFy%2Bkq3Q%3D%3D"

#define DEVID		"user1"//设备ID

u8 flag;
extern unsigned char esp8266_buf[128];
extern u8 humidityH;	  //湿度整数部分
extern u8 humidityL;	  //湿度小数部分
extern u8 temperatureH;   //温度整数部分
extern u8 temperatureL;   //温度小数部分

 u8 LED0_FLAG;
 u8 LED1_FLAG;
//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	//打印一下信息产品id,鉴权信息，设备ID
   printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	//printf("%d\r\n",MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket));
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)//如果平台返回数据不为空则
		{
			//printf("1\r\n");
		//dataPtr = esp8266_buf;
			printf("%s\r\n",dataPtr);
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)//	MQTT数据接收类型判断（connack报文）
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))//打印是否连接成功及连接失败的原因
				{
					case 0:printf( "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:printf(  "WARN:	连接失败：协议错误\r\n");break;
					case 2:printf(  "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf(  "WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf(  "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf(  "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:printf(  "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern u8 temp,humi;
extern u16 light,rain,Pre,angle,servo2_angle;
extern float ppm,aqi,CO2;
extern uint8_t fan_mode;				
extern uint8_t fan_status;
extern uint8_t led_mode;				
extern uint8_t led_status;
extern uint8_t lock_state;

uint16_t OneNet_FillBuf(char *buf)
{
	
	char text[64];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"lock_state\":{\"value\":%s},",lock_state?"true":"false");
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp\":{\"value\":%d.%d},",temperatureH,temperatureL);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"humi\":{\"value\":%d.%d},",humidityH,humidityL);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"ppm\":{\"value\":%.1f},", ppm);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Light\":{\"value\":%d},", light);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"AQI\":{\"value\":%.1f},", aqi);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"CO2\":{\"value\":%.1f},", CO2);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"AMT\":{\"value\":%d},", Pre);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"rain\":{\"value\":%d},", rain);
	strcat(buf, text);
	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"servo\":{\"value\":%d},", angle); 
//	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"servo1\":{\"value\":%d}", Servo2_Status); 
	strcat(buf, text);
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"beep\":{\"value\":%s},", Beep_Status ? "true" : "false");
//	strcat(buf, text);
	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"ledmode\":{\"value\":%s},", led_mode ? "true" : "false");
//	strcat(buf, text);	
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"led\":{\"value\":%s}",led_status ? "true" : "false");
//	strcat(buf, text);
	
	strcat(buf, "}}");
	
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	static char buf[512];
	
	short body_len = 0, i = 0;
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);			//获取当前需要发送的数据流的总长度
	
	printf( "---%s---",buf);
	printf( "---%d---",body_len);

	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVID, body_len, NULL, &mqttPacket) == 0)				//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			printf( "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			printf( "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNET_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}
// ==========================================================
// 函数名称：OneNET_SendPropertySetReply
// 函数功能：发送属性设置响应
// 入口参数：cmd_id - 指令ID
//           code - 响应代码
//           msg - 响应消息
// 返回参数：无
// 说明：必须发送这个响应，否则OneNET会超时
// ==========================================================
void OneNET_SendPropertySetReply(const char *cmd_id, int code, const char *msg)
{
	int a = 0;
    MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};
    char topic_buf[128];
    char payload_buf[256];
    
    // 构建响应主题：在原主题后加 _reply
    snprintf(topic_buf, sizeof(topic_buf), 
             "$sys/%s/%s/thing/property/set_reply", 
             PROID,DEVID);
    
    // 构建响应JSON
    snprintf(payload_buf, sizeof(payload_buf),
		"{\"id\":\"%s\",\"code\":%d,\"msg\":\"%s\"}",
             cmd_id, code, msg);

    
    //printf("发送set_reply响应: topic=%s, payload=%s\r\n", topic_buf, payload_buf);
	

   // OneNET_Publish(topic_buf, payload_buf);
    // 发布响应，使用QoS0（OneNET要求快速响应）
	
    if( (a = MQTT_PacketPublish(MQTT_PUBLISH_ID, topic_buf, payload_buf, 
                         strlen(payload_buf), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet)) == 0)
    {
        ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);
        MQTT_DeleteBuffer(&mqtt_packet);
    }
    else
    {
		printf("%d\r\n",a);
        printf("WARN: 发送set_reply响应失败\r\n");
    }


    
    // 尝试直接发送RAW MQTT数据（如果平台支持）
    // 或者使用更简单的MQTT客户端测试
}

// ==========================================================
// 函数名称：OneNET_SendPropertyPost
// 函数功能：发送属性上报
// 入口参数：property - 属性名
//           value - 属性值
// 返回参数：无
// ==========================================================
//void OneNET_SendPropertyPost(const char *property, int value)
//{
//    MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};
//    char topic_buf[64];
//    char payload_buf[256];
//    static unsigned int msg_id = 1000;  // 消息ID计数器
//    
//    // 构建上报主题
//    snprintf(topic_buf, sizeof(topic_buf),
//             "$sys/%s/%s/thing/property/post",
//             PROID, DEVID);
//    
//    // 获取当前时间戳（如果没有RTC，使用系统运行时间）
//    // 这里使用递增的消息ID作为简单的时间戳
//    msg_id++;
//    
//    // 构建上报JSON
//    snprintf(payload_buf, sizeof(payload_buf),
//             "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":{\"%s\":{\"value\":%s}}}",
//             msg_id,
//             property,
//             value ? "true" : "false");
//    
//    printf("上报属性: topic=%s, payload=%s\r\n", topic_buf, payload_buf);
//    
//    // 发布上报，使用QoS1确保送达
//    if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic_buf, payload_buf, 
//                         strlen(payload_buf), MQTT_QOS_LEVEL1, 0, 1, &mqtt_packet) == 0)
//    {
//        ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);
//        MQTT_DeleteBuffer(&mqtt_packet);
//    }
//    else
//    {
//        printf("WARN: 上报属性失败\r\n");
//    }
//}

//==========================================================
//	函数名称：	OneNET_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[56];
	const char *topic = topic_buf;
	const char *topic1 = topic_buf;
	
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVID);
	
	printf( "Subscribe Topic: %s\r\n", topic_buf);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================

void OneNet_RevPro(unsigned char *cmd)
{
	//printf("1\r\n");
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
//	cJSON *raw_json, *params_json, *servo_json , *servo1_json ,*fanmode_js , *fan_js, *ledmode_js , *led_js;
	cJSON *raw_json, *params_json,*door_json,*id_item,*servo1_json;
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				char *data_ptr = NULL;
				
				printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				raw_json = cJSON_Parse(req_payload);
				id_item = cJSON_GetObjectItem(raw_json, "id");
				const char *cmd_id = id_item->valuestring;
                printf("指令ID: %s\r\n", cmd_id);
                        
                // 2. 立即发送响应（必须在执行控制前发送，避免超时）
				delay_ms(10);
                OneNET_SendPropertySetReply(cmd_id, 200, "success");
				
				
				params_json = cJSON_GetObjectItem(raw_json,"params");//提取params
				
				door_json = cJSON_GetObjectItem(params_json,"lock_state");//提取servo
				if(door_json != NULL)
				{
					lock_state = (door_json->type == cJSON_True) ? 1 : 0;
					PWM_Set();
				}
			
				
//				
//				servo_json = cJSON_GetObjectItem(params_json,"servo");//提取servo
//				if(servo_json != NULL)
//				{
//					if (servo_json->type == cJSON_Number) 
//					{
//						angle = servo_json->valueint;
//						if (angle >= 0 && angle <= 180) 
//						{ // ????????
//							SERVO_SetAngle(angle);         // ????????
//							printf( "Servo set to %d degrees\r\n", angle);
//						} 
//						else 
//						{
//							printf( "Error: Invalid servo angle (%d)\r\n", angle);
//						}
//					}
//				}
//				
//				fanmode_js = cJSON_GetObjectItem(params_json , "fanmode");
//				if(fanmode_js != NULL)
//				{
//					fan_mode = fanmode_js->valueint;
//				}
//				
//				fan_js = cJSON_GetObjectItem(params_json , "fan");
//				if(fan_js != NULL)
//				{
//					fan_status = fan_js->valueint;
//				}
//				
//				ledmode_js = cJSON_GetObjectItem(params_json , "ledmode");
//				if(ledmode_js != NULL)
//				{
//					led_mode = ledmode_js->valueint;
//				}
//				
//				led_js = cJSON_GetObjectItem(params_json , "led");
//				if(led_js != NULL)
//				{
//					led_status = led_js->valueint;
//				}
//				
				servo1_json = cJSON_GetObjectItem(params_json,"servo1");//提取servo
				if(servo1_json != NULL)
				{
					if (servo1_json->type == cJSON_Number) 
					{
						servo2_angle = servo1_json->valueint;
						
						if (servo2_angle >= 0 && servo2_angle <= 100) 
						{ // ????????
							Set_Servo2_Angle(servo2_angle);         // ????????
							printf( "Servo1 set to %d degrees\r\n", servo2_angle);
						} 
						else 
						{
								printf( "Error: Invalid servo angle (%d)\r\n", servo2_angle);
						}
					}
				}
				cJSON_Delete(raw_json);
//				
			}
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf( "Tips:	MQTT Subscribe OK\r\n");
			else
				printf( "Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}








/*================数据流=====================*/
//unsigned char OneNet_FillBuf(char *buf)
//{
//	
//	char text[32];
//	
//	LED0_FLAG=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);//读取LED的开关状态（即对应引脚的）
//	LED1_FLAG=GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);
//	
//	printf("LED0_FLAG_TYPE=%d\n",sizeof(LED0_FLAG));
//	
//	memset(text, 0, sizeof(text));
//	
//	strcpy(buf, ",;");
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "Tempreture,%d.%d;",temperatureH,temperatureL);
//	strcat(buf, text);
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "Humidity,%d.%d;", humidityH,humidityL);
//	strcat(buf, text);
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "LED0,%d;", LED0_FLAG);
//	strcat(buf, text);
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "LED1,%d;", LED1_FLAG);
//	strcat(buf, text);
//	printf("buf_mqtt=%s\r\n",buf);
//	return strlen(buf);

//}




//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================数据流================================
//void OneNet_SendData(void)
//{
//	
//	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
//	
//	char buf[128];
//	
//	short body_len = 0, i = 0;
//	
//	printf( "Tips:	OneNet_SendData-MQTT\r\n");
//	
//	memset(buf, 0, sizeof(buf));//清空数组内容
//	
//	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
//	
//	if(body_len)
//	{
//		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
//		{
//			for(; i < body_len; i++)
//				mqttPacket._data[mqttPacket._len++] = buf[i];
//			
//			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
//			printf( "Send %d Bytes\r\n", mqttPacket._len);
//			
//			MQTT_DeleteBuffer(&mqttPacket);															//删包
//		}
//		else
//			printf(  "WARN:	EDP_NewBuffer Failed\r\n");
//	}
//	
//}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//========================数据流==================================
//void OneNet_RevPro(unsigned char *cmd)
//{
//	
//	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
//	
//	char *req_payload = NULL;
//	char *cmdid_topic = NULL;
//	
//	unsigned short req_len = 0;
//  unsigned char type = 0;

//	short result = 0;

//	char *dataPtr = NULL;
//	char numBuf[10];
//	int num = 0;

//	
//	cJSON *json , *json_value;
//  cJSON *json1, *json_value1;


//	type = MQTT_UnPacketRecv(cmd);
//	switch(type)
//	{
//		case MQTT_PKT_CMD:															//命令下发
//			
//			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
//			if(result == 0)
//			{
//				//打印收到的信息
//				printf(  "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
//				
//				// 对数据包req_payload进行JSON格式解析
//				json = cJSON_Parse(req_payload);
//				
//				if (!json)//如果json内容为空，则打印错误信息
//					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
//				else
//				{
//					json_value = cJSON_GetObjectItem(json , "LED0");//提取对应属性的数值
//  				//	printf("json_value: [%s]\r\n",json_value->string);//转化为字符串数值
//  				//	printf("json_value: [%d]\r\n",json_value->valueint);//转化为数值型数值
//					
//					if((json_value->valueint)==1)
//						//flag=1;//关灯
//						LED0 = 1;
//					else if((json_value->valueint)==0)					
//						//flag=2;//开灯
//						LED0 = 0;

//				}
//				//同上
//				json1 = cJSON_Parse(req_payload);
//				if (!json1)
//					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
//				else
//				{
// 					json_value1 = cJSON_GetObjectItem(json1 , "LED1");

//					if((json_value1->valueint)==1)//整数值
//						//flag=3;//关灯
//					LED1=1;
//					else if((json_value1->valueint)==0)
//						//flag=4;//开灯
//					LED1=0;
//				
//				}
//				
//				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
//				{
//					printf( "Tips:	Send CmdResp\r\n");
//					
//					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
//					MQTT_DeleteBuffer(&mqttPacket);									//删包
//				}
//				cJSON_Delete(json);//释放位于堆中cJSON结构体内存
//				cJSON_Delete(json1);
//			}
//		
//		break;
//			
//		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
//		
//			if(MQTT_UnPacketPublishAck(cmd) == 0)
//				printf(  "Tips:	MQTT Publish Send OK\r\n");
//			
//		break;
//		
//		default:
//			result = -1;
//		break;
//	}
//	
//	ESP8266_Clear();									//清空缓存
//	
//	if(result == -1)
//		return;
//	
//	dataPtr = strchr(req_payload, '}');					//搜索'}'

//	if(dataPtr != NULL && result != -1)					//如果找到了
//	{
//		dataPtr++;
//		
//		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
//		{
//			numBuf[num++] = *dataPtr++;
//		}
//		numBuf[num] = 0;
//		
//		num = atoi((const char *)numBuf);				//转为数值形式
//	}

//	
//	
//	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
//	{
//		MQTT_FreeBuffer(cmdid_topic);
//		MQTT_FreeBuffer(req_payload);
//	}

//}
