#include "RQMutex.h"

RQMutex::RQMutex()
{
}

RQMutex::~RQMutex()
{
}

void RQMutex::Lock(bool isRead)
{
	mutex.lock();
}

void RQMutex::Unlock(bool isRead)
{
	mutex.unlock();
}
