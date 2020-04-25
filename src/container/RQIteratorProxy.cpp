#include "RecycleBin.h"
#include "container/RQIteratorProxy.h"
#include "container/RQLineStorage.h"

RQIteratorProxy::RQIteratorProxy(RQLineStorage* storage)
{
	_lock = new RQSpinLock;
	_storage = storage;
	_iteratorList = RecycleBin::GetFreeRQLoadNode();
}

RQIteratorProxy::~RQIteratorProxy()
{
	delete _lock;
	RecycleBin::RecycleNode(_iteratorList);
}

void RQIteratorProxy::Insert(RQIterator * iterator)
{
	if (iterator)
	{
		RQAutoLock lock(_lock);
		RQLoadNode* current = RecycleBin::GetFreeRQLoadNode();
		current->_obj = iterator;

		RQLoadNode* next = (RQLoadNode*)_iteratorList->_next;

		_iteratorList->_next = current;
		current->_next = next;
	}
}

void RQIteratorProxy::Erase(RQIterator * iterator)
{
	RQLoadNode* target = 0;
	RQLoadNode** nextpp = (RQLoadNode**)&_iteratorList->_next;
	while (*nextpp)
	{
		if ((*nextpp)->_obj == (void*)iterator)			///以传入的iterator为参照寻找已存储的迭代器
		{
			target = *nextpp;
			*nextpp = (RQLoadNode*)(*nextpp)->_next;
			break;
		}
		nextpp = (RQLoadNode**)&(*nextpp)->_next;
	}

	if (target)
	{
		RecycleBin::RecycleNode(target);
	}

}

void RQIteratorProxy::AddIterator(RQIterator * iterator)
{
	if (iterator)
	{
		_storage->AddIteator(iterator);
	}
}

void RQIteratorProxy::SubIterator(RQIterator * iterator)
{
	if (iterator)
	{
		_storage->AddIteator(iterator);
	}
}
