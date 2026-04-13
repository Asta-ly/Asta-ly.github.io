#include "password.h"
#include "w25q128.h"
#include "oled.h"
#include "key.h"
#include "string.h"
#include "stdio.h"
#include "pwm.h"
#include "beep.h"
#include "delay.h"

#define PASSWORD_SIZE   10
uint8_t pwd_input[PASSWORD_SIZE] = {0};
uint8_t pwd_read[PASSWORD_SIZE] = {0};
uint8_t i = 0, key_value = 0, try_times = 0;
extern void door(void);
//初始化函数
void password_init(void)
{
    W25Qxx_Init();
}

//清空输入缓存
void password_input_clear(void)
{
    memset(pwd_input, 0, PASSWORD_SIZE);
    i = 0;
}

//保存密码
void password_save(void)
{
    W25Qxx_Erase_Sector(0x000000);
    W25Qxx_WriteData(0x000000, pwd_input, PASSWORD_SIZE);
    oled_show_changed();
}

//获取键盘输入
uint8_t password_get_input(void)
{
    password_input_clear();
	//printf("111111\r\n");
    while(1)
    {
		//printf("111111\r\n");
       key_value = button_scan();
		//printf("%d\r\n",key_value);
		
        if(key_value == POUND_KEY)
        {
            printf("按下了#号键，input: %s\r\n", pwd_input);
            return POUND_KEY;
        }
        else if(key_value == STAR_KEY)
        {
            printf("按下了*号键\r\n");
            return STAR_KEY;
        }
        else if(key_value != 17)
        {
            printf("按下了：%c\r\n", key_value);
            OLED_ShowChar(20 + i * 10, 4, key_value, 16);
            pwd_input[i++] = key_value;
        }
    }
}

//密码比对
uint8_t password_compare(void)
{
    uint8_t i = 0;
    W25Qxx_ReadData(0x000000, pwd_read, PASSWORD_SIZE);
    
    if(strlen((char *)pwd_input) != strlen((char *)pwd_read))
        return FALSE;
    
    for(i = 0; i < strlen((char *)pwd_read); i++)
    {
        if(pwd_input[i] != pwd_read[i])
            return FALSE;
    }
    
    return TRUE;
}

//密码输入正确的操作
void password_input_right_action(void)
{
    oled_show_right();
 //   lock_on();
	lock_state = 1;
	PWM_Set();
      Beep_Set(1);
    delay_ms(300);
      Beep_Set(0);
    delay_ms(1000);
 //   lock_off();
	lock_state = 0;
	PWM_Set();
    try_times = 0;
}

//密码输入错误的操作
void password_input_wrong_action(void)
{
    oled_show_wrong();
    try_times++;
    if(try_times >= 3)
    {
        Beep_Set(1);
        delay_ms(1000);
        Beep_Set(0);
        try_times = 0;
    }
    delay_ms(1000);
}

//旧密码输入正确的操作
void password_old_right_action(void)
{
    oled_show_new();
    password_get_input();
    password_save();
    
     Beep_Set(1);
    delay_ms(300);
    Beep_Set(0);
    delay_ms(500);
}

//旧密码输入错误的操作
void password_old_wrong_action(void)
{
    oled_show_wrong();
    delay_ms(1000);
}

//检查密码是否存在
void password_check(void)
{
    W25Qxx_ReadData(0x000000, pwd_read, PASSWORD_SIZE);
	for(int i = 0; pwd_read[i] !='\0';i++)
		printf("请出密码：%d\r\n", pwd_read[i]);
    if(pwd_read[0] == '\0' || pwd_read[0] == 0xFF)
    {
        oled_show_set();
        password_get_input();
        password_save();
    }
}
