#include "RQDBStack.h"
#include <malloc.h>
#include <memory.h>
#include "RQSpinLock.h"

RQDBStack::RQDBStack(RQLock* lock,unsigned int size, RQNode* node)
{
	_stack = new RQStack(new RQList(lock));
	_size = size;
	_virAddr = (void*)*(void**)node;		//获取虚拟地址
	_stack->push(node);						//不要浪费，直接入栈
}

RQDBStack::RQDBStack(RQLock* lock,unsigned int size, void* virAddr)
{
	_stack = new RQStack(new RQList(lock));
	_size = size;
	_virAddr = virAddr;
}

RQDBStack::~RQDBStack()
{
}

void RQDBStack::push(RQNode * node)
{
	if (node&&!node->_saveContainer)			///检查节点是否存在，并且该节点处于自由状态（未处于任务RQ容器中）
	{
		node->Clear();
		_stack->push(node);
	}
}

RQNode * RQDBStack::pop()
{
	RQNode* retNode = _stack->pop();
	if (!retNode)
	{
		retNode = _createNode();
	}
	retNode->Init();
	return retNode;
}


RQNode * RQDBStack::_createNode()
{
	RQNode* node = (RQNode*)malloc(_size);			//分配内存空间
	*(void**)node = _virAddr;						//给节点赋予虚表
	node->Create();									//根据虚表中的对象创建函数进行对象默认创建
	node->Reinit();									//重置对象作为节点时的数据
	return node;
}

void RQDBStack::clear()
{
	_stack->Clear();
}

RQStack * RQDBStack::GetStorage()
{
	return _stack;
}

unsigned int RQDBStack::GetCount()
{
	return _stack->GetCount();
}