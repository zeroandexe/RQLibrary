#pragma once

#pragma pack(push,1)

struct RQRoutaData_Addr
{
	int _ip;		//网络节点Ip值，大端
	short _port;	//网络节点端口值，大端
	void Set(int ip, short port);
};

struct RQRoutaData
{
	RQRoutaData_Addr _addr;
	char _type;		//网络节点类型

	RQRoutaData(int ip =0, short port = 0, char type = 0);
	void Set(int ip, short port, char type);
	void Clear();
};
#pragma pack(pop)