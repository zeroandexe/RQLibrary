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
	_writeLock.Lock();				///���϶�д�������
	_writeFlag = true;				///����д��ǣ���֪�¶�������ǰ��Ҫ������
	while (_readCount);				///�ȴ���ִ�еĶ��������
}

void RQWriteLock::ReadLock()
{
	while (_writeFlag);				///��⵱ǰ�Ƿ���д��������ִ��
	_readCount++;					///���Ӷ�������
}

void RQWriteLock::WriteUnlock()
{
	_writeLock.Unlock();			///�⿪������
	_writeFlag = _writeLock.GetNextId() != _writeLock.GetNowId();	///����Ƿ���д������
}

void RQWriteLock::ReadUnlock()
{
	_readCount--;
}
