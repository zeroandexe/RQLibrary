#pragma once
//#include <sys/socket.h>
//#include <netinet/in.h>			///用于确定协议值
//#include <fcntl.h>
//#include <unistd.h>

/*
	该对象为系统sock资源的封装，提供初始化、文件标识符关闭、重置收发系统缓冲区大小等接口封装
*/
struct sockaddr_in;

struct SockAddr_In
{
	unsigned int _family;
	unsigned int _addr;
	unsigned short _port;
	void operator=(const sockaddr_in& addr);
};

class RQSock
{
public:
	RQSock();
	~RQSock();
	
	//设置一个新的sock替换旧sock
	int SetSock(int newSock);
	
	/*
		将sock初始化为TCP，默认为异步sock
		asyn：设置该sock为异步
	*/
	int socket();
	
	//基于字节长度的接收
	void Close();
	
	//获取对象中sock的数据值
	operator int();
	
	//设置系统缓冲区接收大小
	int SetRecvBuffSize(int size);
	
	//设置系统缓冲区发送大小
	int SetSendBuffSize(int size);

	//开启监听，并指定被监听ip
	int Listen(const char* ip, short port, int listenSize);
	int Listen(int ip, short port, int listenSize);

	//接收监听队列中的sock连接
	int Accept(sockaddr_in* clientAddr = 0);
	int Accept(SockAddr_In& clientAddr);

	//连接目标主机
	int Connect(const char* ip, short port);
	int Connect(int ip, short port);

	//将内存地址中的数据放入sock发送缓冲区
	int Send(char* buff, unsigned int dataLen);

	//将sock接收缓冲区中的数据拷贝到目标内存地址
	int Recv(char* buff, unsigned int buffSize);
	
	//将sock设置为异步sock
	int ToAsy();

	//设置sock重用正被使用的端口
	int ToReusePort();

	//检测sock是否有处于链接状态
	bool IsConnecting();

	//检查sock的值是否有效
	bool IsOk();
private:
	int _sock;						//sock句柄
};

