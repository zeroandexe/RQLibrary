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
	if (!_isSleep)			///���̴߳��ڷ�˯��״̬ʱ��Ч
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
	sleepTimeMs = (sleepTimeMs * !!sleepTimeMs) + !sleepTimeMs;		///������С��ֵΪ1

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
	ע�⣬ѭ���˳����߳��δ���weak״̬
*/
void* RQThread::_RunThread(void * param)
{
	RQThread* obj = (RQThread*)param;
	while (obj->_isContinue)
	{
		obj->_sig.Sub();					///�ź������ģ������߳�˯�߿���

		obj->_isRunFun = true;				///���ñ�־���߳̿�ʼִ���û����ú���
		if (obj->_funPtr)
		{
			obj->_funPtr(obj->_param);
		}
		obj->_isRunFun = false;				///�������б�־�����ڱ�ʾ���ú�����ִ�����

		obj->_sig.Add();					///�ź������ӣ������߳������Ĳ���
	}
	obj->_isContinue = true;
	return 0;
}
