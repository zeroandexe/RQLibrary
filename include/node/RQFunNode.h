#pragma once
#include "node/RQNode.h"
class RQFunNode :
	public RQNode
{
public:
	RQFunNode();
	~RQFunNode();
	virtual void Create(void* param = 0);
	virtual void Clear();

	///void(*_callFun)(RQFunNode* param);			///节点调用函数
	void(*_callFun)(void* param);
	void* _param;					///函数参数
};

