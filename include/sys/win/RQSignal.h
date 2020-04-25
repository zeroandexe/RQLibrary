#pragma once
#include <Windows.h>
#include <atomic>
class RQSpinLock;
class RQSignal {
public:
	/*
		�����ź���
		startCount	[in]	�ź�����ʼ����
		maxCount	[in]	�ź��������źŸ��������������źų����������ź���λ����ֵ
	*/
	RQSignal(unsigned int startCount=0,unsigned int maxCount=0x7ffffffe,const char* name=0);			///Ĭ���źŸ���Ϊ1
	~RQSignal();
	void Add();			///����һ���ź�
	/*
		���ݴ��볬ʱʱ����еȴ��ź�
		outTimesMs	[in]	��ʱ�ȴ�ʱ�䣬��λ���룬Ĭ������
		return				WAIT_OBJECT_0	��ʶ���յ���Ϣ��
							WAIT_TIMEOUT	��ʶ��ʱ
							�����ο��ĵ�
	*/
	DWORD Sub(unsigned int outTimesMs = INFINITE);		

	/*
		�ź�����0
	*/
	void Zero();
private:
	HANDLE _sempHandle;		///�ź������
	std::atomic_int _sigCount;			///��ǰ�źż���
	RQSpinLock* _lock;
};
