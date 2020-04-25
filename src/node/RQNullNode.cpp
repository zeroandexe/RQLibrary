#include "node/RQNullNode.h"
#include "RecycleBin.h"
#include "tool/RQMemory.h"


RQNullNode::RQNullNode(int newArySize)
{
	Create((void*)newArySize);
}
RQNullNode::~RQNullNode()
{
	RecycleBin::RecycleNode(_memory);
}

void RQNullNode::Create(void * param)
{
	_arySize = *(int*)&param;
	_memory = RecycleBin::GetFreeRQMemory();
	_arySize = _arySize > 0 ? _arySize : 5;
	_memory->Resize(sizeof(void*) * (_arySize+1));		//多开辟一个空间防止越界
	_ptrAry =(void**) _memory->GetAddr();
}


void RQNullNode::Clear()
{
	_memory->Clear();
}

void RQNullNode::Resize(int newArySize)
{
	_memory->Resize((newArySize + 1) * sizeof(void*));
	_ptrAry = (void**)_memory->GetAddr();
	_arySize = newArySize;
}


long RQNullNode::GetLong(unsigned int index)
{
	void* ptr = GetPtr(index);
	return *(long*)&ptr;
}

void* RQNullNode::GetPtr(unsigned int index)
{
	void* retPtr = 0;
	if (_arySize > index)
	{
		retPtr = _ptrAry[index];
	}
	return retPtr;
}

void RQNullNode::SetLong(unsigned int index, long val)
{
	SetPtr(index, (void*)val);
}

void RQNullNode::SetPtr(unsigned int index, void* val)
{
	if (_arySize > index)
	{
		_ptrAry[index] = val;
	}
}