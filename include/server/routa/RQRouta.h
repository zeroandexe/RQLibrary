#pragma once
#include "RQSession.h"
#include "RQRoutaTable.h"
#include "RQRoutaData.h"
/*
	该对象为单个路由中心对象，提供与路由中心的交互接口
	在路由协议所有第一个网络数据段都为：源节点数据
*/

class RQRouta 
{
public:
	RQRouta(RQRoutaData* localData, int timeOutS);
	/*
		处理路由数据包
		session		[in]	接收到数据包的会话对象
		pack		[in]	本次会话所得到的数据包
	*/
	virtual void RoutaPack(RQSession* session, RQPack* pack) = 0;

public:
	/*
		将参数信息作为路由节点保存
	*/
	virtual void Insert(int ip, short port, char optType);
	virtual void Insert(RQRoutaData* data);
	/*
		将参数所指向的路由节点移除
	*/
	virtual void Remove(int ip, short port);
	virtual void Remove(RQRoutaNode* node);

	/*
		报告异常路由信息
	*/
	virtual void Error(int ip, short port);
	virtual void Error(RQRoutaNode* node);

	/*
		创建相关类型的路由表数据
	*/
	virtual void CopyTable();

	/*
		根据操作类型获取网络节点
	*/
	RQRoutaNode* GetNodeBy(char optType);

	/*
		根据网络地址获取路由节点
	*/
	RQRoutaNode* GetNodeBy(int ip, short port);
	
	/*
		将本节点登陆到路由网络中
	*/
	virtual void LoginRoutaNet();

	/*
		将本节点重路由网络中移除
	*/
	virtual void LogoutRoutaNet();

	/*
		维持本机在路由网络中的活跃性
	*/
	virtual void DelayRoutaNet();


	//获取路由对象设置的超时时间参数
	int GetTimeOutS();

	//获取路由中的本机地址（数值）
	int GetLocalIp();

	//获取路由对象中的本机服务端口（大端）
	short GetLocalPort();

protected:
	/*
		发往另一个路由节点，要求其将本信息插入到路由表中
		insertIp		[in]	需要被加入到路由表的ip（大端）
		insertPort		[in]	端口（大端）
		insertOptType	[in]	该地址提供的服务
		return			[in]	操作是否成功,
								-1：连接远程失败
								0：操作成功
	*/
	virtual void _SendRoutaInsert(int insertIp, short insertPort, int insertOptType) = 0;

	//期望远端路由节点移除目标网络节点信息
	virtual void _SendRoutaRemove(int removeIp, short removePort);
	//向远端路由中心报告网络中存在有问题的路由节点
	virtual void _SendRoutaError(int errorIp, short errorPort) = 0;
	//向远端路由节点发送数据获取请求
	virtual void _SendRoutaCopy();

	//点对点路由数据通讯
	virtual void _SendRouta(int remoteIp, short remotePort,
		int ip, short port, char type, int protocol, int decorate);

	//单向发送连接超时会话销毁函数
	static void _Destory(void* param);

protected:
	RQRoutaTable _routaTable;		//路由信息记录表
	RQRoutaData	_localData;
	int		_timeOutS;				//允许的路由超时时间
};