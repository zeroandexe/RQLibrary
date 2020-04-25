#include "node/RQFunNode.h"



RQFunNode::RQFunNode()
{
}


RQFunNode::~RQFunNode()
{
}

void RQFunNode::Create(void* param)
{
	Clear();
}

void RQFunNode::Clear()
{
	_callFun = 0;
	_param = 0;
}
