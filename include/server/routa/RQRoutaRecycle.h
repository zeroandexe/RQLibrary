#pragma once
#include "RQDBStack.h"

class RQRoutaNode;

class RQRoutaRecycle 
{
public:
	RQRoutaRecycle();


	RQRoutaNode* GetFreeRQRoutaNode();				///获取一个当前可用的路由节点
	void RecycleRQRoutaNode(RQRoutaNode* node);		///回收路由节点
private:
	static RQDBStack* _dbStack;
};