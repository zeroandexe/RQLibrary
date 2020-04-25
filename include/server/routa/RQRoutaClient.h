#pragma once
#include "RQRouta.h"
#include "RQRoutaData.h"
/*
	该类是路由对象的子类，作为路由网络的客户进行交互
*/

class RQRoutaClient : public RQRouta
{
public:
	/*
		要求在创建路由客户的时候指定服务中心
	*/
	RQRoutaClient(RQRoutaData_Addr* serverAddr,
		RQRoutaData* localData,
		int timeOuts);

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

	/*
		处理路由数据包
		session		[in]	接收到数据包的会话对象
		pack		[in]	本次会话所得到的数据包
	*/
	virtual void RoutaPack(RQSession* session, RQPack* pack);
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
	virtual void _SendRoutaInsert(int insertIp, short insertPort, int insertOptType);

	//向远端路由中心报告网络中存在有问题的路由节点
	virtual void _SendRoutaError(int errorIp, short errorPort);

	//期望远端路由节点移除目标网络节点信息
	virtual void _SendRoutaRemove(int removeIp, short removePort);

	//向远端路由节点发送数据获取请求
	virtual void _SendRoutaCopy();

	//拷贝回调函数
	static void _CopyRoutaTable(RQSession* session, RQPack* pack,RQPackHead* head, void* param);

	//路由在路由网中的激活函数
	static void _Active(void* param);

	static void _CopyTable(void* param);

private:
	int		_serverIp;			//路由服务中心地址
	short	_serverPort;		//路由服务中心端口
	RQLoadNode _activeLoad;		//网络注册函数挂载点
	RQLoadNode _copyLoad;		//路由表拷贝挂载点
};