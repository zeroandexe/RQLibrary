#include "RQThread.h"
#include <pthread.h>
#include <signal.h>
#include "RQSys.h"

RQThread::RQThread()
{
	_funPtr = 0;
	_param = 0;
	_Init();
	Naissance();			///创建立即执行的线程
}

RQThread::~RQThread()
{
	Kill();
}


void RQThread::_Init()
{
	_isContinue = true;
	_isSleep = true;
	_sig.Zero();
}


void RQThread::Kill()
{
	pthread_kill(_hThread._ulong, SIGKILL);
	_hThread._ulong = 0;
	_Init();
}

void RQThread::Naissance()
{
	if (!_hThread._ulong)			///检查当前是否未创建线程
	{
		pthread_create(&_hThread._ulong, NULL, _RunThread, this);
	}
}

void RQThread::Sleep()
{
	RQAutoLock lock(&_lock);
	if (!_isSleep)
	{
		_sig.Sub();
		_isSleep = true;
	}
}

void RQThread::Weak()
{
	RQAutoLock lock(&_lock);
	if (_isSleep)
	{
		_sig.Add();
		_isSleep = false;
	}
}

void RQThread::Exit()
{
	_isContinue = false;
}

void RQThread::SetFun(void(*funPtr)(void *), void * param)
{
	_funPtr = funPtr;
	_param = param;
}

int RQThread::Wait(unsigned int sleepTimeS, unsigned int timeOutS)
{
	int retVal = -1;
	for (unsigned int i = 0; i*sleepTimeS > timeOutS; i++)
	{
		if (_isContinue)
		{
			retVal = 0;
			break;
		}
		RQSleepSec(sleepTimeS);
	}
	return retVal;
}

bool RQThread::IsSleep()
{
	return _isSleep;
}

void* RQThread::_RunThread(void * param)
{
	RQThread* obj = (RQThread*)param;
	while (obj->_isContinue)
	{
		obj->_sig.Sub();					///信号量消耗，用于线程睡眠控制

		if (obj->_funPtr)
		{
			obj->_funPtr(obj->_param);
		}

		obj->_sig.Add();					///信号量增加，用于线程自消耗补偿
	}
	obj->_isContinue = true;
	return 0;
}
