#include "node/RQRegularNode.h"

void RQRegularNode::Create(void * param)
{
	Clear();
}

void * RQRegularNode::Init(void * param)
{
	_status = 0;			//节点未被回收但是暂时无效
	return nullptr;
}

void RQRegularNode::Clear()
{
	_registerTimeMs = 0;
	_timeOutMs = 0;
	_funAddr = 0;
	_param = 0;
	_status = -1;			//节点无效并且被回收
	_isLoop = false;
	_allowActive = false;
}
