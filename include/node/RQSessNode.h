#pragma once
#include "RQNode.h"
class RQSession;
class RQPack;

/*
	会话参数存储节点
*/
class RQSessNode : public RQNode
{
public:

	RQSession*	_session;				///会话对象
	RQPack*		_pack;					///属于本会话对象的数据包
	void*		_param;					///本次操作所携带的附加参数

	virtual void* Init(void* param = nullptr);
};