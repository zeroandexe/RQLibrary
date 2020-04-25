#include "RQJsonHead.h"
#include "RQSpinLock.h"
#include "RecycleBin.h"


RQJsonArray::RQJsonArray()
{
	Create();
}

RQJsonArray::~RQJsonArray()
{
	delete _saveList->getLock();
	delete _saveList;
	RecycleBin::RecycleNode(_indexAry);
}

void RQJsonArray::Create(void * param)
{
	RQJson::Create();
	_saveList = new RQList(new RQSpinLock);
	_indexAry = RecycleBin::GetFreeRQNullNode();
}

void RQJsonArray::Clear()
{
	RQNode* node = _saveList->pop_front();
	while (node)
	{
		RecycleBin::RecycleNode(node);
		node = _saveList->pop_front();
	}
	RQJson::Clear();
}

void * RQJsonArray::Init(void * param)
{
	RQJson::Init();
	return nullptr;
}

void RQJsonArray::push(RQJson * json)
{
	_saveList->push_back(json);
}

RQJson & RQJsonArray::operator[](int index)
{
	RQNode* target = 0;
	if (_saveList->GetCount() > index)
	{
		target = (RQNode*)_indexAry->GetPtr(index);
		//for (int i = 0; i <= index; i++)						///确定下标
		//{
		//	target = _saveList->GetNext(target);			///遍历链表
		//	if (!target)
		//	{
		//		break;
		//	}
		//}
	}
	if (!target)											///检测元素是否为空
	{
		target = _InvalidJson();							///获取无效值代替目标值
	}
	return *(RQJson*)target;
}

int RQJsonArray::Count()
{
	int retInt = 0;
	if (_saveList)
	{
		retInt = _saveList->GetCount();
	}
	return retInt;
}

RQJsonArray::operator bool()
{
	return _saveList;
}

void RQJsonArray::Refurbish()
{
	_indexAry->Resize(_saveList->GetCount());
	RQNode* indexNode = _saveList->GetNext();
	for (int i = 0; indexNode; i++)
	{
		_indexAry->SetPtr(i, indexNode);
		indexNode = _saveList->GetNext(indexNode);
	}
}
