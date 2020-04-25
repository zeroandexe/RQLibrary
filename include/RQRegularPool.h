#pragma once
#include "RQMap.h"
/*
	该对象为单例对象，作用为提供定时服务，执行者为线程池
	版本v1
*/
class RQHnaldePool;
class RQThread;
class RQSpinLock;
class RQList;
class RQRegularNode;
class RQFunNode;

class RQRegularPool
{
public:
	typedef void(*CallbackFun_Type)(void* param);

	static void InitObj();							//初始化定时池
	/*
		注册监听数据和处理函数
	*/
	static void Register(void* label,void* param, RQRegularPool::CallbackFun_Type fun, unsigned int timeOutMs, bool allowActive = true,bool isLoop = false);
	/*
		根据数据移除数据所对应的定时任务，并返回本次定时所注册的函数地址
		故可根据返回值是否非0来判断任务是否被执行
		param	[in]	函数地址
		return	[out]	返回函数地址
						若当前任务被挂起，则返回函数有效地址
						若当前任务已被执行，则返回空
	*/
	static void* Remove(void* param);				//移除对应数据的操作
	/*
		活跃一次已被注册的监听数据
	*/
	static void Active(void* param);


protected:
	static void _ThreadFun(void* param =0);			//线程注册函数
	static void _CallFun(void* funNode);		//调用注册数据的执行函数
private:
	static RQMap<void*, RQRegularNode*>* _idMap;	//数据地址-存储id映射表
	static RQList* _saveList;						//已存入数据存储容器
	static RQThread* _checkThread;					//检测线程
	static RQSpinLock* _lock;
	static time_t _checkStartTime;					//开始检测时间
};