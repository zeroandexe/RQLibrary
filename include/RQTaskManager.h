#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
#include "node/RQDivNode.h"
#include "node/RQTaskNode.h"
#include "node/RQFunNode.h"
/*
	该对象为任务管理器对象，为单例工厂，用于管理所有任务进行分类管理，
	并对失效任务执行Destory函数进行销毁

	任务节点一旦进入设备管理器，那么外界将失去对任务节点的修改权

	任务管理器除提供任务的匹配处理外，还提供非匹配任务的立即执行和异步执行等处理
*/
class RQList;

class RQTaskManager {
public:
	typedef RQMap<int, RQTaskNode*> IdTaskMap_Type;					//id映射表

	class Three {
	public:
		IdTaskMap_Type _idTaskMap;		//设备id-任务节点映射表
		RQList* _taskList;				//该类型的设备存储表
		RQSpinLock _lock;				
	};

	typedef Three IdTaskSave_Type;									//任务存表
	typedef RQMap<int, IdTaskSave_Type> DivTaskMap_Type;			//设备表
	typedef RQMap<int, std::pair<RQSpinLock,RQList*>>	TypeTaskMap_Type;					//任务类型表

public:
	/*
		将传入的任务节点存入，存入任务管理器，存入位置由任务任务节点自生参设决定。
		操作者id（_opterId）	：与类型同时存在
		操作者类型（_opterType）：与id同时存在
		（注意：操作id与类型同时存在(id!=0,type!=0)，则管理器认为该任务需要设备则进行设备匹配
				若操作id与类型同时不存在(id=0,type=0)，则管理器认为该任务不需要设备进行设备匹配，
			将以执行参数为空执行）

		任务类型（_typeType）	：必填
		超时时间（_timeOutMs）	：0表示该任务要求立即匹配设备，若匹配失败则进入销毁状态
									<0表示该任务的有效期直到匹配到设备为止
									>0表示该任务具有时间期限，若在规定时间内未完成设备匹配，将回收该任务
	*/
	static void Insert(RQTaskNode* taskNode);						//将任务节点插入到管理器中
	static void Remove(RQTaskNode* taskNode);						//将任务节点从管理器中移除				
	static RQTaskNode* GetTaskByTaskType(int taskType);				//获取某一类型的任务节点
	static RQTaskNode* GetTaskByDiv(int opterType, int opterId);	//根据传入的设备信息获取任务节点
	static RQTaskNode* GetTaskByDivType(int opterType);				//根据设备类型获取设备对应的执行任务
	//为非管理器中的任务提供异步执行接口
	static void ExecTaskThread(RQTaskNode* taskNode, RQDivNode* divNode);
	static void DestoryTaskThread(RQTaskNode* taskNode, RQDivNode* divNode);
private:
	/*
		立即执行任务匹配完成函数
	*/
	static void _ExecTask(RQTaskNode* taskNode,RQDivNode* divNode);
	/*
		立即执行任务销毁函数
	*/
	static void _DestoryTask(RQTaskNode* taskNode, RQDivNode* divNode);		
	static void _DestoryTask(void* param);			//任务立即销毁函数（供线程池使用）

	/*
		根据任务要求匹配设备
	*/
	static RQDivNode* _FindDiv(int opterId, int opterType);
private:
	///任务异步处理函数
	static void _TaskThread(RQTaskNode* taskNode, RQDivNode* divNode, void(*funAddr)(void*));
	static void _DestoryTaskThread(void * funNode);		//线程池任务销毁执行函数
	static void _ExecTaskThread(void * funNode);			//线程池任务运行函数

};
