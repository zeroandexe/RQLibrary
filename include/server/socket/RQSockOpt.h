#pragma once

struct sockaddr_in;
struct sockaddr;


class RQSockOpt {
public:
	//创建基于IP4的TCP的sock资源
	static int SocketT4();

	//关闭套接字，并回收资源
	static int Close(int sock);

	//从传入的sock中获取新连接（监听sock，客户端地址）
	static int Accept(int listenSock, sockaddr_in* clientAddr);

	//开启监听IP4（监听sock，监听匹配地址，监听端口，监听队列长度）
	static int Listen4(int listenSock, const char* bindAddr, short bindPort, int listenSize);
	static int Listen4(int listenSock, int bindAddr, short bindPort, int listenSize);

	//连接目标主机
	static int Connect(int sock, const char* ip, short port);
	static int Connect(int sock, int ip, short port);

	//发送数据
	static int Send(int sock, void* bufPtr, int dataLen);

	//接收数据
	static int Recv(int sock, void* bufPtr, int bufSize);

	//切换为异步模式（默认转换）
	static int ToAsy(int sock);

	//是否开启端口共享
	static int ToReusePort(int sock);

	//设置接收缓冲区大小
	static int SetRecvBuffSize(int sock,int size);

	//设置发送缓冲区大小
	static int SetSendBuffSize(int sock, int size);

protected:
	//将Ip和端口赋值到地址中，默认参数大端（ip4地址对象，协议簇，ip，端口）
	static void _SetAddr4(sockaddr_in& addr, int family, int ip, short port);
	//参数为小端
	static void _SetAddr4(sockaddr_in& addr, int family, const char* ip, short port);
};