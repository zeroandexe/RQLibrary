#pragma once
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>


class RQSignal {
public:
	//创建信号量对象
	RQSignal(unsigned int startCount = 0);			///默认信号个数为1
	~RQSignal();
	void Add();			///增加一个信号
	void Sub();			///减少一个信号
	void Zero();		///将信号量清零
private:
	/*int _semId;			///信号量对象
	int _key;*/
	sem_t _semId;
};
