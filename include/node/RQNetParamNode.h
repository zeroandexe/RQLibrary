#pragma once
#include "node/RQNode.h"
#include "RQPack.h"
#include "RQSession.h"
/*
	
*/
class RQNetParamNode : public RQNode
{
public:
	virtual void Create(void* param = nullptr);			//数据节点创建
	virtual void Clear();								//提供对节点数据的清除操作

	int		_routaRole;							///被请求的路由网络目标
	RQPack* _pack;								///被请求的网络参数
	int		_optProt;							///操作协议簇
	int		_optDecor;							///操作协议请求
	RQSession::CallFun_Type _recvFun;			///若网络回复所回调的执行函数
	void*	_recvParam;							///接收回调的携带参数
	RQSession::CallFun_Type _destoryFun;		///路由会话被销毁所回调的执行函数
	void*	_destoryParam;						///销毁回调的携带参数
	int		_sessTimeOutSec;					///路由会话的存活时间
};