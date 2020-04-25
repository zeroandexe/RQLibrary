#pragma once
#include <mutex>
#include "tool/lock/RQLock.h"
#include "tool/lock/RQAutoLock.h"
class RQMutex : public RQLock
{
public:
	RQMutex();
	virtual ~RQMutex();
	virtual void Lock(bool isRead = true);
	virtual void Unlock(bool isRead = true);
private:
	std::mutex mutex;
};