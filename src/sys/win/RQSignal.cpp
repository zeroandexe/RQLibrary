#include "sys/win/RQSignal.h"
#include <Windows.h>
#pragma comment(lib,"KERNEL32.LIB")
#include "RQAutoLock.h"
#include "RQSpinLock.h"

RQSignal::RQSignal(unsigned int startCount, unsigned int maxCount,const char* name)
{
	_sempHandle = CreateSemaphoreA(0, startCount, maxCount, name);
	_sigCount = 0;
	_lock = new RQSpinLock;
}

RQSignal::~RQSignal()
{
	CloseHandle(_sempHandle);
}

void RQSignal::Add()
{
	//RQAutoLock _lock(_lock);
	ReleaseSemaphore(_sempHandle, 1, 0);
	_sigCount++;
}

unsigned long RQSignal::Sub(unsigned int outTimesMs)
{
	//RQAutoLock _lock(_lock);
	unsigned long  retVal = WaitForSingleObject(_sempHandle, outTimesMs);
	 _sigCount--;
	 return retVal;
}

void RQSignal::Zero()
{
	RQAutoLock _lock(_lock);
	///消耗正向信号量
	//if (_sigCount > 0)
	//{
	//	Sub();
	//}

	/////补充负方向信号量
	//if (_sigCount < 0)
	//{
	//	
	//}
	if (_sigCount)
	{
		_sigCount = 0 - _sigCount;
		ReleaseSemaphore(_sempHandle, _sigCount, 0);
		_sigCount = 0;
	}
}
