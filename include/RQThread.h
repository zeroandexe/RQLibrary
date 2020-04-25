#pragma once
#include "RQSignal.h"
#include "RQPtrVal.h"
#include "RQSpinLock.h"
/*
	线程对象，线程被创建时都处于睡眠状态，需要调用weak进行唤醒。
	采用SetFun可动态切换被执行函数的执行
*/

class RQThread
{
public:
	RQThread();
	~RQThread();

	void Kill();		//立即杀死线程，被杀后必须调用Naissance重新生成线程对象，才可运行
	void Naissance();	//重生线程,
	void Sleep();		//睡眠线程
	void Weak();		//唤醒线程

	void Exit();		//线程退出通知

	/*
		等待线程将该阶段函数执行完成,
		sleepTimeMs	[in]	间隔睡眠时间，默认1秒为间隔
		timeOutMs	[in]	总超时时间，默认永久有效
		return		[in]	若为超时退出则返回-1，若为非超时退出则为0
	*/
	int Wait(unsigned int sleepTimeS = 1000, unsigned int timeOutS = -1);

	/*
		设置线程执行函数，以及函数执行时的参数
	*/
	void SetFun(void(*funPtr)(void*), void* param);


	/*
		对线程当前是否处于睡眠状态进行检测
	*/
	bool IsSleep();
protected:
	static void* _RunThread(void* param);			//线程函数外壳
	void _Init();				//初始化基本参数

private:
	RQSignal _sig;		//线程信号
	void(*_funPtr)(void*);
	void* _param;				//线程函数的执行参数
	RQPtrVal _hThread;			//线程对象的资源句柄
	RQSpinLock _lock;		
	bool _isContinue;			//线程继续执行标志
	bool _isSleep;				//线程睡眠检测标志
	bool _isRunFun;				//线程当前是否在执行设置函数标志
};