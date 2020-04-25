#pragma once
#include "node/RQNode.h"

/*
	该对象为挂载对象，用于在不添加和修改挂在对象的情况下，使对象能够使用RQ容器
*/

class RQLoadNode :
	public RQNode
{
public:
	RQLoadNode();
	virtual ~RQLoadNode();
	virtual void Create(void* param=0);
	virtual void Clear();

	void* _obj;							//挂载节点所挂载的数据
	RQLoadNode* _loadNode;				//另一个挂载自身的挂载节点
};