#pragma once
#include "RQNode.h"
/*
	该节点为活跃值记录节点
*/

struct RQAliveNode  : public RQNode
{
	unsigned long long _lastTimeSec;	//该节点最新活跃时间
	int _isBlack;						//该节点是否被标志为黑名单
	void* _obj;							//该节点所挂载的对象数据

	virtual void Clear();
	virtual void* Init(void* param = 0);
};