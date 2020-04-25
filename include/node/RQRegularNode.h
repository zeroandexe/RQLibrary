#pragma once
#include "node/RQNode.h"
#include "RQMutex.h"
#include <atomic>
/*
	该程序用于定时池对定时任务记录
	版本：v1
*/
typedef void(*ThreadFun_Type)(void* param);

class RQRegularNode : public RQNode
{
public:
	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();
	unsigned long long _registerTimeMs;		//注册入定时池的时间
	unsigned int _timeOutMs;				//预定的超时时间
	ThreadFun_Type _funAddr;				//预定任务执行函数接口
	void* _param;							//本次任务执行的参数
	void* _label;							//标签识别参数
	/*
		节点是否已经准备好
		-1：节点无效可销毁
		0：节点有效但不可访问
		1:节点有效并可访问

	*/
	std::atomic_int _status;
	bool _isLoop;							//是否进行循环操作
	bool _allowActive;						//是否允许被激活
};