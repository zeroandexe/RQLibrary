#include "compiler/RQASTNode.h"
#include "define/RQASTDefine.h"

RQASTNode::RQASTNode()
{
	Create();
}


RQASTNode::~RQASTNode()
{
}

void RQASTNode::Create(void* param)
{
	Clear();
}

void RQASTNode::Clear()
{
	_left = (RQASTNode*)0;
	_right = (RQASTNode*)0;
	_parent = (RQASTNode*)0;
	_value = (RQVar*)0;
	_token = M_SYMBOL_TOKEN_NULL;
	_oldToken = M_SYMBOL_PRIORITY_NULL;

}