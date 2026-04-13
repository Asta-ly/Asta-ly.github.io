#ifndef _ONENET_H_
#define _ONENET_H_

_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

void OneNET_Subscribe(void);

void OneNET_SendPropertySetReply(const char *cmd_id, int code, const char *msg);
void OneNET_Publish(const char *topic, const char *msg);

#endif
