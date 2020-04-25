#include "RQRecyBin.h"
#include "RQDBStack.h"
#include "RQSpinLock.h"
#include "RQQueue.h"
#include "RQLoadNode.h"
#include "RQList.h"

std::map<void*, RQDBStack*>* RQRecyBin::_recyBin = 0;
RQDBStack* RQRecyBin::_loadNodeDbStack;
RQList* RQRecyBin::_freeTemStorage;


RQSpinLock* RQRecyBin::_lock;									//防止创建暂存器时出现竞争
int RQRecyBin::_nextTsId = 1;										//下一个暂存器id

void RQRecyBin::Init()
{
	if (!_recyBin)
	{
		_recyBin = new std::map<void*, RQDBStack*>;
		_freeTemStorage = new RQList(new RQSpinLock);
		_lock = new RQSpinLock();
		_loadNodeDbStack = new RQDBStack(new RQSpinLock, sizeof(RQLoadNode), (RQNode*)new RQLoadNode);
	}
}

void RQRecyBin::Create(void * virAddr, unsigned int size, RQLock * lock)
{
	RQDBStack** dbstack = &(*_recyBin)[virAddr];

	if (!*dbstack)				//检查目标回收器是否不存在
	{
		(*dbstack) = new RQDBStack(new RQSpinLock, size, virAddr);
	}
}

void RQRecyBin::Create(RQNode * node, unsigned int size, RQLock * lock)
{
	RQDBStack** dbstack = &(*_recyBin)[*(void**)&node];

	if (!*dbstack)				//检查目标回收器是否不存在
	{
		(*dbstack) = new RQDBStack(lock, size, node);
	}
}

void* RQRecyBin::CreateTS()
{
	RQLoadNode* oldLoad = (RQLoadNode*)_freeTemStorage->pop_back();		///获取一个旧的暂存器
	if (!oldLoad)		///检查当前是否存在旧的暂存器
	{
		RQQueue* newQueue = RQQueue::Create_list(new RQSpinLock);				///没有空闲的暂存器，需要重新构建
		oldLoad = (RQLoadNode*)_loadNodeDbStack->pop();					///获取一个挂载节点将暂存器挂载
		oldLoad->_obj = newQueue;
	}
	return oldLoad;
}

void RQRecyBin::TemStorage(void* tsId, RQNode * node)
{
	RQQueue* queue = TsToQue(tsId);							///从传入的句柄中获取对应的暂存器
	
	RQLoadNode* loadNode_node = (RQLoadNode*)_loadNodeDbStack->pop();
	loadNode_node->_obj = node;
	node->_param = loadNode_node;
	queue->push_back(loadNode_node);						///将传入的节点进行存储
}

void RQRecyBin::RemoveFromTs(void * tsId, RQNode * node)
{
	if (node
		&& node->_param)
	{
		RQQueue* queue = TsToQue(tsId);
		queue->Remove((RQNode*)node->_param);
	}
}

void RQRecyBin::RecycleTSNode(void* tsId)
{
	RQQueue* queue = TsToQue(tsId);

	///回收指定暂存器中的所有元素
	RQLoadNode* loadNode_node = (RQLoadNode*)queue->pop_front();		///从暂存器中获取一个回收挂载节点
	while (loadNode_node)
	{
		Recycle((RQNode*)loadNode_node->_obj);					///回收挂载节点上的回收目标对象
		_loadNodeDbStack->push(loadNode_node);				///回收挂载节点
		loadNode_node = (RQLoadNode*)queue->pop_front();	///取出下一个将被回收的挂载节点
	}
}

void RQRecyBin::RecycleTS(void* tsId)
{
	RQLoadNode* loadNode = (RQLoadNode*)tsId;
	_freeTemStorage->push_back(loadNode);					///将当前不在使用的暂存器存入空闲暂存器容器中
}

void RQRecyBin::Recycle(RQNode * node)
{
	if (node&&!node->_saveContainer)
	{
		void* virAddr = *(void**)node;			//获取节点的虚拟指针
		RQDBStack* dbstack = (*_recyBin)[virAddr];

		if (dbstack)			//检查回收器是否存在，若存在则进行继续回收，否则泄漏
		{
			dbstack->push(node);
		}
	}
}

RQNode* RQRecyBin::GetFree(void * virAddr)
{
	RQNode* retNode = 0;
	RQDBStack* dbstack = (*_recyBin)[virAddr];
	if (dbstack)			//检查回收器是否存在，若存在则进行继续回收，否则泄漏
	{
		retNode = dbstack->pop();
	}
	return retNode;
}

RQQueue * RQRecyBin::TsToQue(void * tsId)
{
	RQLoadNode* loadNode_queue = (RQLoadNode*)tsId;
	return (RQQueue*)loadNode_queue->_obj;			///从传入的句柄中获取对应的暂存器
}
