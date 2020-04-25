#pragma once
#include <Windows.h>
#include <atomic>
class RQSpinLock;
class RQSignal {
public:
	/*
		创建信号量
		startCount	[in]	信号量初始个数
		maxCount	[in]	信号量最大的信号个数，运行中若信号超过上限则信号数位上限值
	*/
	RQSignal(unsigned int startCount=0,unsigned int maxCount=0x7ffffffe,const char* name=0);			///默认信号个数为1
	~RQSignal();
	void Add();			///增加一个信号
	/*
		根据传入超时时间进行等待信号
		outTimesMs	[in]	超时等待时间，单位毫秒，默认阻塞
		return				WAIT_OBJECT_0	标识接收到信息号
							WAIT_TIMEOUT	标识超时
							其他参考文档
	*/
	DWORD Sub(unsigned int outTimesMs = INFINITE);		

	/*
		信号量归0
	*/
	void Zero();
private:
	HANDLE _sempHandle;		///信号量句柄
	std::atomic_int _sigCount;			///当前信号计量
	RQSpinLock* _lock;
};
