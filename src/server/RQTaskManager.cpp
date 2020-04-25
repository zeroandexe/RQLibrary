#include "RQTaskManager.h"
#include "RQThreadPool.h"
#include "RecycleBin.h"
#include "RQRegularPool.h"
#include "RQPair.h"
#include "RQList.h"
#include "RQLoadNode.h"
#include "RQDivManager.h"
#include "server/RQTaskContainer.h"

/*
	taskNode为空则取消后续操作
	若有正确的设备信息，
		1、存在设备信息，则进行设备匹配，
			若匹配成功则立即执行该任务
				若匹配失败则查看是否具有超时设置，若有则加入定时池（超时销毁）
					若没有则执行销毁任务操作
		2、若没有设备信息，则无视设备直接执行任务
	若没有正确的设备信息，则执行销毁任务操作
*/
void RQTaskManager::Insert(RQTaskNode * taskNode)
{
	///[0]：函数结束时将执行的任务处理函数；[1]：任务匹配成功函数；[2]：任务匹配失败执行函数
	static void(*_runFun[2])(RQTaskNode*, RQDivNode*) = { _ExecTask,_DestoryTask};

	if (!taskNode||taskNode->_saveContainer)				///若传入空指针或者
	{
		return;
	}
	int taskOpt = 0;								///任务执行操作
	RQDivNode* targetDivNode = 0;					///目标执行任务设备
	void(*_fun)(RQTaskNode*, RQDivNode*) = _ExecTask;///默认本次任务为立即执行任务

	///检测是否需要设备执行，若需要则为0，若不需要则为2，其他为参数不全
	taskOpt = !taskNode->_opterId + !taskNode->_opterType;
	if (!taskOpt)									///检测需要是设备执行
	{
		targetDivNode = _FindDiv(taskNode->_opterId, taskNode->_opterType);	///根据任务要求查找设备
		if (!targetDivNode)							///设备不存在，但是给予超时等待设备匹配
		{
			if (taskNode->_timeOutMs > 0)			///检查当前任务是否决定被超时销毁，即任务是否永久有效
			{
				RQRegularPool::Register(taskNode,taskNode, _DestoryTask, taskNode->_timeOutMs);
			}
			RQTaskContainer::Insert(taskNode);						///按照任务节点参数设置，将任务节点存入管理器
			_fun = 0;								///任务已交给定时池，所以这里不再进行即时调用
		}
		else {
			_fun = _runFun[!targetDivNode];			///根据目标节点是否存在决定所调用的函数
		}
	}
	if (_fun)										///检查当前是否存在需要执行的函数
	{
		_fun(taskNode, targetDivNode);				///将任务节点和目标设备作为参数进行执行
	}
}

void RQTaskManager::Remove(RQTaskNode * taskNode)
{
	RQTaskContainer::Remove(taskNode);
}

RQTaskNode * RQTaskManager::GetTaskByTaskType(int taskType)
{
	return RQTaskContainer::GetTaskByTaskType(taskType);
}

RQTaskNode * RQTaskManager::GetTaskByDiv(int opterType, int opterId)
{
	return RQTaskContainer::GetTaskByDivTypeId(opterType, opterId);
}

RQTaskNode * RQTaskManager::GetTaskByDivType(int opterType)
{
	return RQTaskContainer::GetTaskByDivType(opterType);
}

void RQTaskManager::ExecTaskThread(RQTaskNode * taskNode, RQDivNode * divNode)
{
	_TaskThread(taskNode, divNode, _ExecTaskThread);
}

void RQTaskManager::DestoryTaskThread(RQTaskNode * taskNode, RQDivNode * divNode)
{
	_TaskThread(taskNode, divNode, _DestoryTaskThread);	
}

/*
	任务匹配成功或者要求立即执行（非线程异步）
*/
void RQTaskManager::_ExecTask(RQTaskNode * taskNode,RQDivNode* divNode)
{
	taskNode->Execute(divNode);				///调用任务的匹配成功函数，或立即执行函数
}

/*
	任务匹配失败不等待或任务参数无效立即执行销毁函数
	非入存储器，即可不移除
*/
void RQTaskManager::_DestoryTask(RQTaskNode * taskNode, RQDivNode * divNode)
{
	taskNode->Destroy(divNode);			///由于立即任务匹配失败，锁着任务参数错误调用销毁函数
}

void RQTaskManager::_TaskThread(RQTaskNode * taskNode, RQDivNode * divNode, void(*funAddr)(void *))
{
	RQFunNode* funNode = RecycleBin::GetFreeRQFunNode();
	RQPair* pair = RecycleBin::GetFreeRQPair();

	pair->_first._ptr = taskNode;
	pair->_second._ptr = divNode;
	RQThreadPool::Push(funAddr,pair);
}

/*
	进入线程池执行销毁函数
*/
void RQTaskManager::_DestoryTaskThread(void * param)
{
	RQPair* pair = (RQPair*) param;
	((RQTaskNode*)pair->_first._ptr)->Destroy((RQDivNode*)pair->_second._ptr);	///超时销毁调用函数

	RecycleBin::RecycleNode(pair);
}

void RQTaskManager::_ExecTaskThread(void * param)
{
	RQPair* pair = (RQPair*)param;
	((RQTaskNode*)pair->_first._ptr)->Execute((RQDivNode*)pair->_second._ptr);	///超时销毁调用函数

	RecycleBin::RecycleNode(pair);
}

//如存储器后执行
void RQTaskManager::_DestoryTask(void * param)
{
	RQTaskNode* taskNode = (RQTaskNode*)param;
	Remove(taskNode);
	taskNode->Destroy(0);
}

RQDivNode * RQTaskManager::_FindDiv(int opterId, int opterType)
{
	RQDivNode* divNode = RQDivManager::GetDivByTypeId(opterType, opterId);
	if (!divNode)
	{
		divNode = RQDivManager::GetDivByType(opterType);
	}
	return divNode;
}