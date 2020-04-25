#pragma once
#include "tool/lock/RQLock.h"
#include "tool/lock/RQAutoLock.h"
#include <atomic>
/*
提供自旋锁操作
*/

class RQSpinLock : public RQLock
{  
public:
	RQSpinLock();
	~RQSpinLock();

	virtual void Lock(bool isRead = true);
	virtual void Unlock(bool isRead = true);
	int GetNextId() { return _nextId; }
	int GetNowId() { return _nowId; }
private:
	volatile std::atomic<int> _nextId;
	volatile int _nowId;
};