#pragma once 
#include "RQSpinLock.h"
#include <atomic>
/*
	该对象为写锁：
	1、数据可进行多读，在数据读时不允许写操作
	2、期望进行写操作时，后续新执行读操作将被阻塞，现有读操作将继续直到完毕
	3、写操作同时只能存在一个
*/

class RQWriteLock : public RQLock
{
public:
	RQWriteLock();
	~RQWriteLock();
	virtual void Lock(bool isRead = true);
	virtual void Unlock(bool isRead = true);
private:
	void WriteLock();
	void ReadLock();
	void WriteUnlock();
	void ReadUnlock();

private:
	RQSpinLock _writeLock;
	volatile std::atomic<int> _readCount;
	volatile bool _writeFlag;
};