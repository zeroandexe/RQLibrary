#pragma once
#include "node/RQNode.h"
#include "RQPtrVal.h"

/*
	句柄对象，作为RQ容器的通用句柄对象
*/
class RQHandleServer;
class RQHandle :public RQNode
{
public:
	virtual void Create(void* param = 0);
	virtual void Clear();

	RQPtrVal _handle;
	RQHandleServer* _handlServer;
};