#pragma once
#include "RQCommunication.h"
#include "server/routa/RQRoutaTable.h"
#include "server/routa/RQRoutaCenter.h"
#include "server/routa/RQRoutaClient.h"
#include "node/RQNetParamNode.h"

class RQRoutaNet
{
public:
	/*
		设置路由服务中心地址以及服务中心程序端口，并设置本机外网地址以及本机程序端口
		(数值型参数都为大端模式)
		routa			[in]	本机在路由网络中的角色
		recvPortFun		[in]	用户数据的接收处理函数
		funParam		[in]	接收处理函数所携带的处理参数
	*/
	RQRoutaNet(RQRouta* routa, RQSession::CallFun_Type recvPortFun,void* funParam);
	~RQRoutaNet();

	/*
		登陆路由网络，并通知本机地址端口以及所提供的服务，同时开启本机对外的服务
		serverAddr	[in]	路由服务中心地址
		return		[out]	成功发出登陆
							未执行登 陆知返回-1
	*/
	int LoginNet(int serviceNum);

	/*
		本机关闭对路由网络的服务，并通知网络本机离线
	*/
	void LogoutNet();

	/*
		向网络请求完成指定操作类型的服务，并交所约定的数据交付于网络。
		optType			[in]	本次请求的服务类型
		pack			[in]	本次需要处理的数据包
		recvFun			[in]	当网络处理完成后，处理网络回应数据的回调函数
		recvFunParam	[in]	接收函数的携带参数
		destroyFun		[in]	当会话销毁时的处理函数
		destoryFunParam	[in]	会话销毁函数的携带参数
		timeOutS		[in]	路由会话的存活时间/超时时间
	*/
	int OptNet(int optType, RQPack* pack,
		int optProtocol,int optDecorate,
		RQSession::CallFun_Type recvFun,void* recvFunParam,
		RQSession::CallFun_Type destroyFun,void* destoryFunParam,
		int timeOutS);

protected:
	/*
		开启服务监听
		port		[in]	指定提供服务的端口（大端模式）
		waitNum		[in]	监听队列最大长度
		return		[out]	成功开启服务则返回0，否则非0
	*/
	int _ListenService(short port, int waitNum);

private:
	//网络数据接收处理函数
	static void _RecvFun(RQSession* session, RQPack* pack,RQPackHead* head, void * param);
	//服务端口对新连接的接受回调
	static void _AcceptFun(RQSession* session, void * param, SockAddr_In& addr);
private:
	RQSession* _listenSession;				//服务监听端口
	RQRouta* _routa;						//本网络对象所在的路由角色
	RQSession::CallFun_Type _userRecvFun;	//用户定义用户协议处理函数
	void* _userParam;						//用户接收函数参数
};