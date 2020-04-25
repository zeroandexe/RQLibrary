#include "RQAlivePool.h"
#include "RQArray.h"
#include "RQStack.h"
#include "RQSpinLock.h"
#include <time.h>

union Ptr_Int{
	void* _ptr;
	int _val;
};

struct RQActPoolElement {
	unsigned long long _actTime;		///元素最近活跃时间
	long _timeOutSec;					///元素允许的超时时间
	int _saveFlag;						///元素的存储标识符
	int _aliveFlag;						///元素的活跃标识符
	void* _param;						///元素数据   
};

RQAlivePool::RQAlivePool(unsigned int poolSize)
{
	_poolSize = poolSize + 1;							///第0号元素不分配
	_pool = new RQActPoolElement[_poolSize];			///
	_stackSaveIndex = new RQStack(new RQArray(poolSize,new RQSpinLock)); 	///创建存储下标存储栈
	_stackAlivePool = new RQStack(new RQArray(poolSize, new RQSpinLock));	///创建活跃栈
	for (unsigned int i = 1; i < _poolSize; i++)				///补充存储元素下标
	{
		_stackSaveIndex->push((RQNode*)i);						///将可用下标存入数据中
	}
}

RQAlivePool::~RQAlivePool()
{
	delete[] _pool;
	delete _stackSaveIndex;
	delete _stackAlivePool;
}

int RQAlivePool::AddParam(int hPool, void * param, long timeOut)
{
	Ptr_Int retHPool;
	retHPool._val = -1;
	if (hPool >=0 && hPool < _poolSize)								///防止无效数据，传入
	{
		retHPool._val = hPool;
		if (hPool)													///传入有效句柄，标识对现有元素进行更新
		{
			_ActParam(hPool, param, timeOut);
			if (!_pool[hPool]._aliveFlag)							///检查元素当前是否在活跃池中
			{
				_stackAlivePool->push((RQNode*)retHPool._val);		///将参数加入活跃池
				_pool[retHPool._val]._aliveFlag = 1;
			}
		}
		else {														///元素不存在则返回
			retHPool._ptr = _stackSaveIndex->pop();
			if (retHPool._val)											///防止空数据
			{
				_ActParam(retHPool._val, param, timeOut);				///设置监控数据
				_pool[retHPool._val]._saveFlag = 1;
				_stackAlivePool->push((RQNode*)retHPool._val);		///将参数加入活跃池
				_pool[retHPool._val]._aliveFlag = 1;
			}
		}
	}
	return retHPool._val;
}

void * RQAlivePool::GetAlive()
{
	void* retParam = 0;
	int isOk;
	Ptr_Int hPool;
	hPool._ptr = _stackAlivePool->pop();
	while(hPool._val)
	{
		isOk = _pool[hPool._val]._saveFlag * (_pool[hPool._val]._actTime > (time(0) - _pool[hPool._val]._timeOutSec));
		_pool[hPool._val]._aliveFlag = 0;
		if (isOk)				///检查当前元素是否依然在被使用
		{
			retParam = _pool[hPool._val]._param;
			break;
		}
		hPool._ptr = _stackAlivePool->pop();
	} 

	return retParam;
}

void RQAlivePool::Remove(int hPool)
{
	if (hPool > 0 && hPool < _poolSize)
	{
		_pool[hPool]._saveFlag = 0;
		_stackSaveIndex->push((RQNode*)hPool);
	}
}

void RQAlivePool::_ActParam(int hPool, void * param, long timeOut)
{
	_pool[hPool]._param = param;
	_pool[hPool]._actTime = time(0);
	_pool[hPool]._timeOutSec = timeOut;
}