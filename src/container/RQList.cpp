#include "tool/lock/RQAutoLock.h"
#include "RQList.h"
#include "RecycleBin.h"

#define M_CONNECT(prevNode,nextNode)	\
{prevNode->_next = nextNode; nextNode->_prev = prevNode;}

//在将node插入在prev和next之间
#define M_INSERT(prevNode,node,nextNode)				\
{_Insert(prevNode,node,nextNode);}
///{M_CONNECT(prevNode,node);M_CONNECT(node,nextNode);node->_saveContainer=this;_count++;}

//将node从链表的关联中移除
#define M_INIT(node)				\
{node->_next = nullptr; node->_prev = nullptr;}

//将node从prev和next中取出来
#define M_REMOVE(prevNode,node,nextNode)		\
{_Remove(prevNode,node,nextNode);}
///{M_CONNECT(prevNode,nextNode);M_INIT(node);node->_saveContainer = 0;_count--;}

RQList::RQList(RQLock* lock) 
{
	_head = new RQNode;
	_head->_prev = nullptr;				//初始化头节点
	_head->_saveContainer = this;

	_end = new RQNode;
	_end->_next = nullptr;				//初始化位节点
	_end->_saveContainer = this;

	M_CONNECT(_head, _end);
	_lock = lock;
	_visitNode = _head;
	_count = 0;
}


RQList::~RQList()
{
	Clear();
	delete _head;
	delete _end;
}

void RQList::Clear()
{
	RQAutoLock lock(_lock);
	RQNode** iterPtr = &_head->_next;
	while (*iterPtr != _end)
	{
		RQNode* freeNode = *iterPtr;
		freeNode->_saveContainer = 0;				///清空自由节点归属

		*iterPtr = freeNode->_next;
		RecycleBin::RecycleNode(freeNode);		///回收节点
	}
	_end->_prev = _head;
	_count = 0;
}

void RQList::Destroy()
{
	RQAutoLock lock(_lock);
	RQNode** iterPtr = &_head->_next;
	while (*iterPtr != _end)
	{
		RQNode* freeNode = *iterPtr;
		*iterPtr = freeNode->_next;

		delete freeNode;
	}
	_end->_prev = _head;
	_count = 0;
}

unsigned int RQList::GetCount()
{
	return _count;
}
void RQList::Remove(RQNode * node)
{
	RQAutoLock lock(_lock);
	if (node
		&&this == node->_saveContainer)				///检测节点存在，并且节点属于该容器
	{
		if (node->_next
			&&node->_prev)					///检查节点是否在容器中
		{
			M_REMOVE(node->_prev, node, node->_next);	///将node从链表中移除，并node的连接指针置空
		}
	}
}

RQLock * RQList::getLock()
{
	return _lock;
}

void RQList::InsertByCmp(RQNode * targetNode, bool(*cmpFun)(RQNode *, RQNode *))
{
	RQAutoLock lock(_lock);
	if (!targetNode->_saveContainer)
	{
		RQNode* basicNode = _head->_next;
		while (basicNode != _end)
		{
			if (cmpFun(basicNode, targetNode))
			{
				break;
			}
			basicNode = basicNode->_next;
		}
		M_INSERT(basicNode->_prev, targetNode, basicNode);
	}
}

RQIterator RQList::begin()
{
	return RQIterator(_proxy, _head->_next);
}

RQIterator RQList::end()
{
	return RQIterator(_proxy, _end);
}

void RQList::AddIteator(RQIterator * iterator)
{
	if (iterator)
	{
		RQAutoLock lock(_lock);
		int isOk = ((**iterator) - _end)*((**iterator)->_next - _end);
		if (!isOk)		///检测当前元素是否已在或者将到达链表尾
		{
			**iterator = _end;
		}
		else {
			**iterator = (**iterator)->_next;	///若未到达则获取下一个有效节点位置
		}
	}
}

void RQList::SubIterator(RQIterator * iterator)
{
	if (iterator)
	{
		RQAutoLock lock(_lock);
		int isOk = ((**iterator) - _head)*((**iterator)->_prev - _head);
		if (!isOk)		///检测当前元素是否已在或者将到达链表头
		{
			**iterator = _head;
		}
		else {
			**iterator = (**iterator)->_prev;	///若未到达则获取上一个有效节点位置
		}
	}
}

void RQList::Erase(RQIterator * iterator)
{
	if (**iterator)
	{
		if (iterator->IsWon(_proxy))		///检测迭代器是否和链表是合作关系
		{
			RQAutoLock lock(_lock);
			RQNode* ptr = **iterator;
			int isOk = (ptr - _head)*(ptr - _end)*(ptr - (RQNode*)0);
			if (isOk)						///检测迭代器所在元素位置是否有效
			{
				RQAutoLock lock(_lock);
				**iterator = ptr->_prev;	///将目标节点的上一节点赋予迭代器
				M_REMOVE(ptr->_prev, ptr, ptr->_next);	///将期望移除的节点从链表中移除
			}
		}
	}
}

RQNode * RQList::LoopVisit()
{
	RQNode* retNode = 0;
	RQAutoLock lock(_lock);
	if (_visitNode->_saveContainer != this
		|| _visitNode->_next == _end)		///检测遍历拜访锚点，保证锚点一直位于本容器对象内
	{
		_visitNode = _head;
	}
	
	if (_visitNode->_next != _end)
	{
		_visitNode = _visitNode->_next;
		retNode = _visitNode;
	}

	return retNode;
}

void RQList::_Insert(RQNode * prev, RQNode * node, RQNode * next)
{
	M_CONNECT(prev, node);
	M_CONNECT(node, next);
	node->_saveContainer = this;
	_count++; 
}

void RQList::_Remove(RQNode * prev, RQNode * node, RQNode * next)
{
	M_CONNECT(prev, next);
	M_INIT(node);
	node->_saveContainer = 0;
	_count--;
}

void RQList::push_back(RQNode * node)
{
	RQAutoLock lock(_lock);
	if (node
		&&!node->_saveContainer)					///要求能够被存入的节点存在，且属于自由节点
	{
		M_INSERT(_end->_prev, node, _end);			///在end前插入一个节点
	}
}

RQNode * RQList::pop_back()
{
	RQNode* target = nullptr;
	RQAutoLock lock(_lock);
	if (_head->_next != _end)						///检查是否还存在有效节点
	{
		target = _end->_prev;
		M_REMOVE(target->_prev, target, target->_next);
	}
	return target;
}

void RQList::push_front(RQNode * node)
{
	RQAutoLock lock(_lock);
	if (node&&!node->_saveContainer)
	{
		M_INSERT(_head, node, _head->_next);			//在head后插入一个节点
	}
}

RQNode * RQList::pop_front()
{
	RQNode* target = nullptr;
	RQAutoLock lock(_lock);
	if (_head->_next != _end)				//检查是否还存在有效节点
	{
		target = _head->_next;
		M_REMOVE(target->_prev, target, target->_next);
	}
	return target;
}

RQNode * RQList::GetNext(RQNode * node)
{
	RQNode* retNode = 0;
	RQAutoLock lock(_lock);
	if (!node||node->_saveContainer == this)
	{
		node = node ? node : _head;			///检测空起点，若是空则标识从头遍历
		node = node->_next;					///获取目标节点的下一个节点
		retNode = node == _end ? 0 : node;	///检测是否已完成节点的遍历，若遍历完成则返回为空
	}
	return retNode;
}