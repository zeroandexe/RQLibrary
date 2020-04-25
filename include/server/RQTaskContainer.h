#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
class RQTaskNode;
class RQList;

/*
	该对象为任务管理器的任务存储模块，负责任务的存储，为单例工厂

	存储器的输入，要求管理器已进过检测
*/

class RQTaskContainer
{
private:
	friend class RQTaskManager;

	typedef RQMap<int, RQTaskNode*> IdTaskMap_Type;					//id映射表

	class Three {
	public:
		IdTaskMap_Type _idTaskMap;		//设备id-任务节点映射表
		RQList* _taskList;				//该类型的设备存储表
		RQSpinLock _lock;
	};

	typedef Three IdTaskSave_Type;									//任务存表
	typedef RQMap<int, IdTaskSave_Type> DivTaskMap_Type;			//设备表
	typedef RQMap<int, std::pair<RQSpinLock, RQList*>>	TypeTaskMap_Type;	//任务类型表
private:
	static void Insert(RQTaskNode* taskNode);			//将任务存入存储器
	static void Remove(RQTaskNode* taskNode);			//将任务从管理器中移除
	static RQTaskNode* GetTaskByTaskType(int taskType);	//根据传入的任务类型获取任务（该任务将被移除）
	static RQTaskNode* GetTaskByDivTypeId(int divType, int divId);	//根据传入的设备类型和id获取匹配任务（一旦获取，任务将被移除）
	static RQTaskNode* GetTaskByDivType(int divType);			//根据传入的设备类型，获取匹配类型的任务
private:
	static void _InsertDivTaskMap(RQTaskNode* taskNode);		//将任务插入设备表
	static void _InsertTaskTypeMap(RQTaskNode* taskNode);		//将任务插入任务类型表
	static void _RemoveDivTaskMap(RQTaskNode* taskNode);		//将任务从设备表中移除
	static void _RemoveTaskTypeMap(RQTaskNode* taskNode);		//将设备从任务类型表中移除
private:
	static DivTaskMap_Type _divTaskMap;							//设备信息为主键的任务表
	static TypeTaskMap_Type _typeTaskMap;						//任务类型为主键的任务表
};