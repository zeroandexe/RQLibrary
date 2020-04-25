#include "RQThread.h"
#include "RQSys.h"
RQThread::RQThread()
{
	_param = 0;
	_funPtr = 0;

	_Init();
	Naissance();
}


void RQThread::_Init()
{
	_isContinue = true;
	_isSleep = true;
	_sig.Zero();
}


RQThread::~RQThread()
{
	Kill();
}

void RQThread::Kill()
{
	CloseHandle(_hThread._ptr);
	_hThread._ptr = 0;
	_Init();
}

void RQThread::Naissance()
{
	if (!_hThread._ptr)
	{
		_hThread._ptr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_RunThread, this, 0, NULL);
	}
}

void RQThread::Sleep()
{
	if (!_isSleep)			///仅线程处于非睡眠状态时有效
	{
		_sig.Sub();
		_isSleep = true;
	}
}

void RQThread::Weak()
{
	if (_isSleep)
	{
		_sig.Add();
		_isSleep = false;
	}
}

void RQThread::Exit(bool isWait)
{
	_isContinue = false;
	if (isWait)
	{
		while (!_isContinue);
	}
	_Init();
	
}

void RQThread::SetFun(void(*funPtr)(void *), void * param)
{
	_funPtr = funPtr;
	_param = param;
}

void RQThread::Wait(unsigned int sleepTimeMs, unsigned int timeOutMs)
{
	sleepTimeMs = (sleepTimeMs * !!sleepTimeMs) + !sleepTimeMs;		///设置最小阈值为1

	for (unsigned int i = 0; i*sleepTimeMs > timeOutMs; i++)
	{
		if (!_isRunFun)
		{
			break;
		}
		RQSleepMs(sleepTimeMs);
	}
}

bool RQThread::IsSleep()
{
	return _isSleep;
}

/*
	注意，循环退出后，线程任处于weak状态
*/
void* RQThread::_RunThread(void * param)
{
	RQThread* obj = (RQThread*)param;
	while (obj->_isContinue)
	{
		obj->_sig.Sub();					///信号量消耗，用于线程睡眠控制

		obj->_isRunFun = true;				///设置标志：线程开始执行用户设置函数
		if (obj->_funPtr)
		{
			obj->_funPtr(obj->_param);
		}
		obj->_isRunFun = false;				///设置运行标志：用于表示设置函数已执行完毕

		obj->_sig.Add();					///信号量增加，用于线程自消耗补偿
	}
	obj->_isContinue = true;
	return 0;
}
