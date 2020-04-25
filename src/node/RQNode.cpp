#include "node/RQNode.h"
#include "RQList.h"
#include "RQLoadNode.h"
/*
	这里不允许create、init、clear之间进行相互调用
*/

RQNode::RQNode()
{
	Reinit();
}

void RQNode::Create(void * param)
{
	Reinit();
}

void * RQNode::Init(void * param)
{
	Clear();      
	return nullptr;
}

void RQNode::Clear()
{
}

void RQNode::Destroy()
{
	
}

void RQNode::Reinit()
{
	_saveContainer = 0;
	_param = 0;
}
