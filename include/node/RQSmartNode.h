#pragma once
#include "RQNode.h"
#include <atomic>
#include "RQSpinLock.h"
/*
	该对象为只能指针共享数据的存储的存储对象
	并提供销毁数据存储、数据的引用计数、以及销毁函数的调用
	默认提供数据的delete操作
*/

class RQSmartNode : public RQNode 
{
public:
	RQSmartNode();
	~RQSmartNode();
	typedef void(*ReleseFun_Type)(void*);
	virtual void Create(void* param = 0);
	virtual void Clear();

	void AddRef();
	bool SubRef();
	void* GetPtr();
	void SetPtr(void* ptr);
	void SetFun(ReleseFun_Type releaseFun);

private:
	void _ClearData();
private:
	RQSpinLock*		_lock;
	void*			_ptr;			//保存数据的指针
	std::atomic_int _count;			//计数值
	ReleseFun_Type _releaseFun;	//数据释放函数
};