#include "tool/lock/RQAutoLock.h""
#include "tool/lock/RQLock.h"

RQAutoLock::RQAutoLock(RQLock * lock,bool isRead)
{
	_lock = lock;
	_isRead = isRead;
	_isLock = false;

	if (_lock)
	{
		_lock->Lock(_isRead);
		_isLock = true;
	}
}

void RQAutoLock::UnLock()
{
	if (_lock&& _isLock)
	{
		_lock->Unlock(_isRead);
		_isLock = false;
	}
}


RQAutoLock::~RQAutoLock()
{
	if (_isLock)
	{
		UnLock();
	}
}