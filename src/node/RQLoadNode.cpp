#include "RQLoadNode.h"



RQLoadNode::RQLoadNode()
{
	Create();
}


RQLoadNode::~RQLoadNode()
{
}

void RQLoadNode::Create(void* param)
{
	Clear();
}

void RQLoadNode::Clear() 
{
	_obj = 0;
	_loadNode = 0;
}

