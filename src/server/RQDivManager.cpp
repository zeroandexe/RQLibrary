#include "RQDivManager.h"
#include "RQList.h"
#include "RQRegularPool.h"
#include "RQThreadPool.h"
#include "RQTaskManager.h"
#include "define/RQdefine.h"
#include "server/RQDivContainer.h"

//RQMap<int, RQDivManager::Three> RQDivManager::_typeDivMap;
//RQMap<int, RQDivNode*> RQDivManager::_sockDivMap;


void RQDivManager::Insert(RQDivNode * divNode)
{
	if (!divNode || divNode->_isRegister)	///要求设备节点未进入管理器管理
	{
		return;
	}
	if(RQDivContainer::Insert(divNode))		///将设备节点存入容器，并获取存入情况
	{
		_MatchTask(divNode);				///根据设备参数匹配任务
	}
}

void RQDivManager::Remove(RQDivNode * divNode)
{
	if (divNode&& divNode->_isRegister)		///检查传入设备存在，且该设备存于管理器
	{
		if (divNode->_timeOutMs>0)			///检查该设备是否在定时池中注册过
		{
			RQRegularPool::Remove(divNode);	///将该设备从定时池中移除
		}
		RQDivContainer::Remove(divNode);	///将设备从管理器中移除
	}
}

void RQDivManager::Remove(int divType, int divId)
{
	RQDivNode* divNode = RQDivContainer::GetDivByTypeId(divType,divId);		///根据给定的设备信息获取设备节点
	Remove(divNode);	///将设备节点移除
}

RQDivNode * RQDivManager::GetDivByTypeId(int divType, int divId)
{
	RQDivNode* divNode = RQDivContainer::GetDivByTypeId(divType, divId);		///根据给定的设备信息获取设备节点
	if (divNode)		///检查索要设备是否存在
	{
		RQDivContainer::WillRunTask(divNode);	///期望设备存在，通知存储器该设备将用于任务执行
	}
	return divNode;		///返回设备获取结果
}

RQDivNode * RQDivManager::GetDivByType(int divType)
{
	RQDivNode* divNode = RQDivContainer::GetDivByType(divType);	///获取期望类型的设备，不关心设备号
	if (divNode)
	{
		RQDivContainer::WillRunTask(divNode);	///期望设备存在，通知存储器该设备将用于任务执行
	}
	return divNode;
}

void RQDivManager::ChangeSock(RQDivNode * divNode, int socket)
{
	if (divNode&&divNode->_isRegister)
	{
		RQDivContainer::ChangeSock(divNode, socket);
	}
}

void RQDivManager::ChangeSock(int divType, int divId,int socket)
{
	RQDivNode* divNode = RQDivContainer::GetDivByTypeId(divType, divId);
	RQDivContainer::ChangeSock(divNode, socket);
}

void RQDivManager::ActiveDivNode(RQDivNode * divNode)
{
	if (divNode&&divNode->_isRegister)			///查看设备是否有效，并且该设备已存在于管理器中
	{
		if (divNode->_timeOutMs > 0)			///检测当前设备确实存在，并且设备为具有激活要求
		{
			if (M_DIV_STATUS_INVALID != divNode->_divStatus)	///检查任务是否已经由于未激活及时而导致失效
			{
				RQRegularPool::Register(divNode,divNode, _DestoryDiv, divNode->_timeOutMs);	///重新激活设备
			}
		}
	}
}

void RQDivManager::ActiveDivNode(int divType, int divId)
{
	///根据给定的设备信息获取设备节点
	RQDivNode* saveDiv = RQDivContainer::GetDivByTypeId(divType,divId);
	if (saveDiv)
	{
		_ActiveDivNode(saveDiv, saveDiv);
	}
}

void RQDivManager::TaskOver(RQDivNode * divNode)
{
	if (divNode && divNode->_isRegister)
	{
		if (M_DIV_STATUS_WORK == divNode->_divStatus)		///并检查当前设备是否为正常工作结束
		{
			if (!_MatchTask(divNode))						///检查任务是否匹配失败，若失败则表明当前设备没有可用任务
			{
				RQDivContainer::TaskOver(divNode);			///该设备已无任务可执行，故通知存储该设备已空闲
			}
		}
	}
}

void RQDivManager::TaskOver(int divType, int divId)
{
	RQDivNode* saveDiv = RQDivContainer::GetDivByTypeId(divType,divId);
	TaskOver(saveDiv);
}

RQTaskNode * RQDivManager::_FindTask(int divType, int divId)
{
	RQTaskNode* taskNode = RQTaskManager::GetTaskByDiv(divType, divId);	///优先获取指定本设备的任务
	if (!taskNode)
	{
		taskNode = RQTaskManager::GetTaskByTaskType(divType);	///指定设备任务没有获取到，那么就获取符合该设备执行的某类任务
	}
	return taskNode;
}


bool RQDivManager::_MatchTask(RQDivNode * divNode)
{
	RQTaskNode* task = _FindTask(divNode->_divType, divNode->_divId);		///获取符合该设备的执行任务
	if (task)							///检测是否存在该任务
	{
		RQDivContainer::WillRunTask(divNode);
		RQTaskManager::ExecTaskThread(task, divNode);		///由于找到了匹配设备的任务则将其交由任务管理器进行处理
	}
	return task;
}

void RQDivManager::_ActiveDivNode(RQDivNode * saveDiv, RQDivNode * targetDiv)
{
	if (saveDiv == targetDiv
		&&saveDiv->_timeOutMs > 0)		///检测当前设备确实存在，并且设备为具有激活要求
	{
		if (M_DIV_STATUS_INVALID != saveDiv->_divStatus)	///检查任务是否已经由于未激活及时而导致失效
		{
			RQRegularPool::Register(saveDiv,saveDiv, _DestoryDiv, saveDiv->_timeOutMs);	///重新激活设备
		}
	}
}

void RQDivManager::_DestoryDiv(void * param)
{
	RQDivNode* divNode = (RQDivNode*)param;
	Remove(divNode);
	divNode->_isRegister = false;
	divNode->Destory();
}
