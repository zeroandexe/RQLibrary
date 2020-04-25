#pragma once
#include "RQSignal.h"
#include "RQPtrVal.h"
#include "RQSpinLock.h"
/*
	�̶߳����̱߳�����ʱ������˯��״̬����Ҫ����weak���л��ѡ�
	����SetFun�ɶ�̬�л���ִ�к�����ִ��
*/

class RQThread
{
public:
	RQThread();
	~RQThread();

	void Kill();		//����ɱ���̣߳���ɱ��������Naissance���������̶߳��󣬲ſ�����
	void Naissance();	//�����߳�,
	void Sleep();		//˯���߳�
	void Weak();		//�����߳�

	void Exit();		//�߳��˳�֪ͨ

	/*
		�ȴ��߳̽��ý׶κ���ִ�����,
		sleepTimeMs	[in]	���˯��ʱ�䣬Ĭ��1��Ϊ���
		timeOutMs	[in]	�ܳ�ʱʱ�䣬Ĭ��������Ч
		return		[in]	��Ϊ��ʱ�˳��򷵻�-1����Ϊ�ǳ�ʱ�˳���Ϊ0
	*/
	int Wait(unsigned int sleepTimeS = 1000, unsigned int timeOutS = -1);

	/*
		�����߳�ִ�к������Լ�����ִ��ʱ�Ĳ���
	*/
	void SetFun(void(*funPtr)(void*), void* param);


	/*
		���̵߳�ǰ�Ƿ���˯��״̬���м��
	*/
	bool IsSleep();
protected:
	static void* _RunThread(void* param);			//�̺߳������
	void _Init();				//��ʼ����������

private:
	RQSignal _sig;		//�߳��ź�
	void(*_funPtr)(void*);
	void* _param;				//�̺߳�����ִ�в���
	RQPtrVal _hThread;			//�̶߳������Դ���
	RQSpinLock _lock;		
	bool _isContinue;			//�̼߳���ִ�б�־
	bool _isSleep;				//�߳�˯�߼���־
	bool _isRunFun;				//�̵߳�ǰ�Ƿ���ִ�����ú�����־
};