#include "RQThreadPool.h"
#include "RQSignalQueue.h"
#include "node/RQFunNode.h"
#include "RQLoadNode.h"
#include "RecycleBin.h"
#include "RQThread.h"
#include "RQDBStack.h"
#include "define/RQdefine.h"
#include "RQSys.h"

unsigned int RQThreadPool::_size;					//线程池中线程数量
RQThread* RQThreadPool::_array;						//线程存放数组
RQSignalQueue* RQThreadPool::_queue;				//函数节点存放地点
bool RQThreadPool::_continueFlag;					//线程继续运行标志


void RQThreadPool::CreatePool(unsigned int size)
{
	static bool needInit = true;
	if (needInit)
	{
		_size = size > 0 ? size : GetCpuCoreNum();
		_array = new RQThread[_size];
		_queue = new RQSignalQueue;
		_continueFlag = true;

		_RunThreadPool();				///启动线程池

		needInit = false;
	}
}

void RQThreadPool::Push(RQFunNode * node, bool isFirst)
{
	if (node)
	{
		RQLoadNode* loadNode = (RQLoadNode*)RecycleBin::GetFreeRQLoadNode();
		loadNode->_obj = node;
		if (isFirst)
		{
			_queue->push_front(loadNode);			///将外部函数调用节点加入线程任务队列中
		}
		else {
			_queue->push_back(loadNode);			///将外部函数调用节点加入线程任务队列中
		}
	}
}

void RQThreadPool::Push(void(*callFun)(void *), void * param, bool isFirst)
{
	if (callFun)
	{
		RQFunNode* node = RecycleBin().GetFreeRQFunNode();
		node->_callFun = callFun;
		node->_param = param;
		Push(node, isFirst);
	}
}

void RQThreadPool::Stop(bool needWait)
{
	_continueFlag = false;
	if (needWait)
	{
		_Wait();
	}
}

int RQThreadPool::GetNodeCount()
{
	return _queue->GetCount();
}

void RQThreadPool::Kill()
{
	for (unsigned int i = 0; i < _size; i++)
	{
		_array[i].Kill();
	}
}

void RQThreadPool::_RunThreadPool()
{
	for (unsigned int i = 0; i < _size; i++)
	{
		_array[i].SetFun(_ThreadFunModel, 0);
		_array[i].Weak();
	}
}

void RQThreadPool::_ThreadFunModel(void* obj)
{
	while (_continueFlag)
	{
		///获取一个可执行的函数执行节点
		RQLoadNode* loadNode = (RQLoadNode*)_queue->pop_front();	
		if (loadNode
			&&loadNode->_obj)
		{
			RQFunNode* funNode = (RQFunNode*)loadNode->_obj;
			if (funNode->_callFun)
			{
				funNode->_callFun(funNode->_param);								///执行存入准备放入线程池执行的函数节点
			}
			RecycleBin().RecycleNode((RQFunNode*)loadNode->_obj);				///回收被挂载的函数节点
		}
		RecycleBin::RecycleNode(loadNode);
	}
}

void RQThreadPool::_Wait()
{
	for (unsigned int i = 0; i < _size; i++)
	{
		_array[i].Exit();
		_array[i].Wait();
	}
}

