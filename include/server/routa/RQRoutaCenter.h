#pragma once
#include "RQRouta.h"
/*
	在路由网络中作为路由中心的对象
*/

class RQRoutaCenter : public RQRouta
{
public:
	RQRoutaCenter(RQRoutaData* localData, int timeOutS);

	void Error(RQRoutaNode * node);
	/*
		向表中的所有路由节点
		insertIp		[in]	需要被加入到路由表的ip（大端）
		insertPort		[in]	端口（大端）
		insertOptType	[in]	该地址提供的服务
		return			[in]	操作是否成功
	*/
	virtual void _SendRoutaInsert(int insertIp, short insertPort, int insertOptType);

	//期望远端路由节点移除目标网络节点信息
	virtual void _SendRoutaRemove(int removeIp, short removePort);

	//向远端路由中心报告网络中存在有问题的路由节点
	virtual void _SendRoutaError(int errorIp, short errorPort);
	/*
		处理路由数据包
		session		[in]	接收到数据包的会话对象
		pack		[in]	本次会话所得到的数据包
	*/
	virtual void RoutaPack(RQSession* session, RQPack* pack);
protected:
	//处理路由中的路由表拷贝协议
	void ProtFun_Copy(RQSession* session);
	void ProtFun_Other(void* dataAddr, int dataLen, RQPackHead* head);
};