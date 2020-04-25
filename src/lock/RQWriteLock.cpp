#include "RQWriteLock.h"

RQWriteLock::RQWriteLock()
{
	_readCount = 0;
	_writeFlag = false;
}

RQWriteLock::~RQWriteLock()
{
}

void RQWriteLock::Lock(bool isRead)
{
	if (isRead)
	{
		ReadLock();
	}
	else {
		WriteLock();
	}
}

void RQWriteLock::Unlock(bool isRead)
{
	if (isRead)
	{
		ReadUnlock();
	}
	else {
		WriteUnlock();
	}
}

void RQWriteLock::WriteLock()
{
	_writeLock.Lock();				///加上对写入加上锁
	_writeFlag = true;				///设置写标记，告知新读操作当前需要被阻塞
	while (_readCount);				///等待以执行的读操作完成
}

void RQWriteLock::ReadLock()
{
	while (_writeFlag);				///检测当前是否有写操作期望执行
	_readCount++;					///增加读操作数
}

void RQWriteLock::WriteUnlock()
{
	_writeLock.Unlock();			///解开锁操作
	_writeFlag = _writeLock.GetNextId() != _writeLock.GetNowId();	///检测是否还有写锁存在
}

void RQWriteLock::ReadUnlock()
{
	_readCount--;
}
