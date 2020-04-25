#pragma once
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>


class RQSignal {
public:
	//�����ź�������
	RQSignal(unsigned int startCount = 0);			///Ĭ���źŸ���Ϊ1
	~RQSignal();
	void Add();			///����һ���ź�
	void Sub();			///����һ���ź�
	void Zero();		///���ź�������
private:
	/*int _semId;			///�ź�������
	int _key;*/
	sem_t _semId;
};
