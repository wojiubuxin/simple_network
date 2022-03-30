#ifndef MSG_H
#define MSG_H

//压缩字节对齐
#pragma pack(push, 1)

//最大包处理
#define MAX_PACKAGE_SIZE    1024

//消息ID
#define MISSION_ASK				120
#define MISSION_REPLY			121
#define BUY_ASK						122
#define BUY_REPLY					123

struct msg_header
{
	int packagesize;//指定包体大小
	int msgid;		 //执行程序ID
	msg_header()
	{
		packagesize = 0;
		msgid = 0;
	}
};

//MISSION_ASK

struct msg_mission
{
	char data[10];	 //内容
	msg_mission()
	{
		memset(data, 0, sizeof(data));
	}
};

//MISSION_REPLY
struct msg_mission_rsp
{
	int code;
	char msg[10];
	msg_mission_rsp()
	{
		code = 0;
		memset(msg, 0, sizeof(msg));
	}
};

//BUY_ASK
//BUY_REPLY
struct msg_buy
{
	int itemid;	//物品ID
	int count;		//物品数量
	msg_buy()
	{
		itemid = 0;
		count = 0;
	}
};

//BUY_REPLY
struct msg_buy_rsp
{
	int code;

	msg_buy_rsp()
	{
		code = 0;
	}
};

#pragma pack(pop)


#endif
