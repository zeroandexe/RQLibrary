#include "server/RQTaskContainer.h"
#include "node/RQTaskNode.h"
#include "RQLoadNode.h"
#include "RQList.h"
#include "RecycleBin.h"

RQTaskContainer::DivTaskMap_Type RQTaskContainer::_divTaskMap;							//设备信息为主键的任务表
RQTaskContainer::TypeTaskMap_Type RQTaskContainer::_typeTaskMap;

void RQTaskContainer::Insert(RQTaskNode * taskNode)
{
	if (taskNode && !taskNode->_isRegister)
	{
		taskNode->_isRegister = true;			///将任务标记为已存入管理器
		_InsertDivTaskMap(taskNode);			///将任务存入设备映射表中
		_InsertTaskTypeMap(taskNode);			///将任务存入任务类型映射表中
	}
}

void RQTaskContainer::Remove(RQTaskNode * taskNode)
{
	if (taskNode&&taskNode->_isRegister)	///检测任务节点是否存在归属容器
	{
		_RemoveDivTaskMap(taskNode);		///将任务节点从设备映射表中移除
		_RemoveTaskTypeMap(taskNode);		///将任务节点从任务类型映射表中移除
		taskNode->_isRegister = false;		///标志该任务已不受管理
	}
}

RQTaskNode * RQTaskContainer::GetTaskByTaskType(int taskType)
{
	///从类型存储容器中取出任务节点
	RQList* saveList = _typeTaskMap[taskType].second;
	RQTaskNode* taskNode = 0;
	if (saveList)
	{
		RQLoadNode* loadNode = (RQLoadNode*)saveList->pop_front();
		if (loadNode)			///是否存在任务节点，若存在则需要将其移除
		{
			taskNode = (RQTaskNode*)loadNode->_obj;
			_RemoveDivTaskMap(taskNode);				///将该任务从设备映射表中移除
			taskNode->_loadNode = 0;					///取消任务节点的挂载
			RecycleBin::RecycleNode(loadNode);			///回收挂载节点
		}
	}
	return taskNode;
}

RQTaskNode * RQTaskContainer::GetTaskByDivTypeId(int divType, int divId)
{
	RQTaskNode* taskNode = 0;
	auto& idTaskMap = _divTaskMap[divType]._idTaskMap;
	auto iter = idTaskMap.Find(divId);
	if (iter != idTaskMap.end())
	{
		taskNode = iter->second;
		Remove(taskNode);							///获取到指定任务，并将任务从中移除
	}
	return taskNode;
}

RQTaskNode * RQTaskContainer::GetTaskByDivType(int divType)
{
	auto& third = _divTaskMap[divType];
	RQTaskNode* taskNode = 0;
	if (third._taskList)
	{
		taskNode = (RQTaskNode*)third._taskList->pop_front();
		Remove(taskNode);
	}
	return taskNode;
}

void RQTaskContainer::_InsertDivTaskMap(RQTaskNode * taskNode)
{
	IdTaskSave_Type& third = _divTaskMap[taskNode->_opterType];	///根据设备获取设备id-任务存储容器
	{
		RQAutoLock lock(&third._lock);							///加锁防止链表被重复创建
		if (!third._taskList)									///检查当任务类型是否具有存储链表
		{
			third._taskList = new RQList(new RQSpinLock);		///为当前链表存储
		}
	}
	third._taskList->push_back(taskNode);						///将任务存储该类型的存储链表中
	third._idTaskMap[taskNode->_opterId] = taskNode;			///将任务存入设备id映射表中
}

void RQTaskContainer::_InsertTaskTypeMap(RQTaskNode * taskNode)
{
	RQLoadNode* loadNode = RecycleBin::GetFreeRQLoadNode();		///获取一个用于装载任务节点的挂载节点
	std::pair<RQSpinLock, RQList*>& pair = _typeTaskMap[taskNode->_taskType];
	{
		RQAutoLock lock(&pair.first);
		if (!pair.second)
		{
			pair.second = new RQList(new RQSpinLock);			///若该类型没有存储容器则创建存储容器
		}
	}
	///挂载任务节点
	loadNode->_obj = taskNode;
	taskNode->_loadNode = loadNode;

	pair.second->push_back(loadNode);							///将设备存入存储容器中
}

void RQTaskContainer::_RemoveDivTaskMap(RQTaskNode * taskNode)
{
	IdTaskSave_Type& third = _divTaskMap[taskNode->_opterType];	///根据设备获取设备id-任务存储容器
	if (third._taskList
		&&third._taskList == taskNode->_saveContainer)			///检查任务节点的存储容器是否就时该容器
	{
		third._taskList->Remove(taskNode);					///从容器中移除改任务节点
	}
	third._idTaskMap.Remove(taskNode->_opterId);			///将任务从id映射表中移除
}

void RQTaskContainer::_RemoveTaskTypeMap(RQTaskNode * taskNode)
{
	std::pair<RQSpinLock, RQList*>& pair = _typeTaskMap[taskNode->_taskType];
	///查看该类型是否存在任务存储链表，并且该节点是否属于该链表
	if (pair.second && pair.second == taskNode->_loadNode->_saveContainer)
	{
		pair.second->Remove(taskNode->_loadNode);			///是该列表的任务移除该任务
		RecycleBin::RecycleNode(taskNode->_loadNode);		///回收用于挂载任务节点的挂载节点
		taskNode->_loadNode = 0;
	}
}
