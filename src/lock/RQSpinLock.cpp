#include "RQSpinLock.h"

RQSpinLock::RQSpinLock()
{
	_nowId = 0;
	_nextId = _nowId - 1;
}

RQSpinLock::~RQSpinLock()
{
}

void RQSpinLock::Lock(bool isRead)
{
	int tempId= ++_nextId;
	while (_nowId != tempId);
}


void RQSpinLock::Unlock(bool isRead)
{
	_nowId++;
}