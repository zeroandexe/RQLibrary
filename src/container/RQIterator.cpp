#include "container/RQIterator.h"
#include "container/RQIteratorProxy.h"
RQIterator::RQIterator()
{
	_node = 0;
	Init();
}

RQIterator::RQIterator(RQIteratorProxy * proxy,RQNode* node)
{
	_node = node;
	Init(proxy);
}

RQIterator::~RQIterator()
{
	Clear();
}

RQIterator::RQIterator(const RQIterator & other)
{
	operator=(other);
}

//RQIterator::RQIterator(RQIteratorProxy && proxy)
//{
//	operator=(proxy);
//}

void * RQIterator::Init(void * param)
{
	_proxy = (RQIteratorProxy*)param;
	if (param)
	{
		_proxy->Insert(this);
	}
	return nullptr;
}

void RQIterator::Clear()
{
	if (_proxy)
	{
		_proxy->Erase(this);
	}
	_proxy = 0;
	_node = 0;
}

void RQIterator::operator=(const RQIterator & other)
{
	Clear();
	_node = other._node;
	Init(other._proxy);
}

void RQIterator::operator=(RQIterator && other)
{
	Clear();
	_node = other._node;
	Init(other._proxy);
}

bool RQIterator::operator==(const RQIterator & other)
{
	return _node == other._node;
}

bool RQIterator::operator==(RQIterator && other)
{
	return _node == other._node;
}

bool RQIterator::operator!=(const RQIterator & other)
{
	return _node !=other._node;
}

bool RQIterator::operator!=(RQIterator && other)
{
	return _node !=other._node;
}

bool RQIterator::isNull()
{
	return _node == 0;
}

void RQIterator::operator++()
{
	if (_proxy)
	{
		_proxy->AddIterator(this);		///所决定的实现交给代理决定
	}
}

void RQIterator::operator--()
{
	if (_proxy)
	{
		_proxy->SubIterator(this);
	}
}

RQNode*& RQIterator::operator*()
{
	return _node;
}

bool RQIterator::IsWon(RQIteratorProxy * proxy)
{
	return _proxy == proxy;
}
