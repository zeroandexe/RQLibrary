#include "RQAliveStack.h"
#include "RQList.h"
#include "RQSpinLock.h"
#include "RQDBStack.h"
#include "RQSys.h"
#include "RecycleBin.h"

RQAliveStack::RQAliveStack()
{
	_lock = new RQSpinLock();
	///_saveList = new RQList(new RQSpinLock);	
	_saveList = new RQList(new RQSpinLock());
}

RQAliveStack::~RQAliveStack()
{
	RecycleRQAliveStack(this);			///本容器内的所有活跃节点
	delete _lock;
	delete _saveList->getLock();
	delete _saveList;
}


RQAliveNode * RQAliveStack::pop_alive(unsigned int aliveTimeSec)
{
	unsigned long long curTimeSec = 0;
	LoadNowTimeSec(curTimeSec);				///获取当前最新时间

	RQAutoLock lock(_lock);
	RQAliveNode* node = (RQAliveNode*)_saveList->pop_back();	///从容器中去除最活跃的节点
	if (node
		&&aliveTimeSec)
	{
		if ((curTimeSec - node->_lastTimeSec) > aliveTimeSec)		///检测该节点是否已经死亡
		{
			_saveList->push_back(node);						///节点处于死亡，将节点存放回回收栈
			node = 0;
		}
	}
	return node;
}

RQAliveNode * RQAliveStack::pop_dead(unsigned int aliveTimeSec)
{
	unsigned long long curTimeSec = 0;
	LoadNowTimeSec(curTimeSec);				///获取当前最新时间

	RQAutoLock lock(_lock);
	RQAliveNode* node = (RQAliveNode*)_saveList->pop_front();	///取出死亡最久的节点
	if (node
		&&aliveTimeSec)
	{
		if ((curTimeSec - node->_lastTimeSec) < aliveTimeSec)		///检测该节点是否还处于存活期
		{
			_saveList->push_front(node);						///节点处于存活期，则节点未死亡将节点放回数组
			node = 0;
		}
	}

	return node;
}

void RQAliveStack::push(RQAliveNode * node)
{
	///---检测节点是否为具备在本活跃栈中存在存储性
	if (node
		&& !node->_isBlack
		&& (node->_saveContainer == _saveList
			|| !node->_saveContainer))
	{
		RQAutoLock lock(_lock);
		///---更新节点最新活跃时间
		LoadNowTimeSec(node->_lastTimeSec);

		///---将节点的位置移动到栈顶
		_saveList->Remove(node);
		_saveList->push_back(node);
	}
}

void RQAliveStack::Remove(RQAliveNode * node)
{
	RQAutoLock lock(_lock);
	_saveList->Remove(node);
}

int RQAliveStack::Count()
{
	return _saveList->GetCount();
}

RQAliveNode * RQAliveStack::GetFreeRQAliveNode()
{
	return RecycleBin::GetFreeRQAliveNode();
}

void RQAliveStack::RecycleRQAliveStack(RQAliveStack * aliveStack)
{
	if (aliveStack)
	{
		RQAliveNode* node = aliveStack->pop_dead(0);	///从栈空间中获取空闲节点
		while (node)
		{
			RecyclRQAliveNode(node);					///回收活跃节点
			node = aliveStack->pop_dead(0);				///继续去除活跃节点
		}
	}
}

void RQAliveStack::RecyclRQAliveNode(RQAliveNode * node)
{
	if (node)
	{
		RQAutoLock lock(_lock);
		if (node->_saveContainer == _saveList)
		{
			_saveList->Remove(node);
		}
		else {
			if (!node->_saveContainer)
			{
				RecycleBin::RecycleNode(node);
			}
		}
	}
}
