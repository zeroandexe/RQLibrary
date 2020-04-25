#include "node/RQAliveNode.h"


void RQAliveNode::Clear()
{
	_lastTimeSec = 0;
	_isBlack = true;
	_obj = 0;
}

void * RQAliveNode::Init(void * param)
{
	_lastTimeSec = 0;
	_isBlack = false;
	_obj = 0;
}
