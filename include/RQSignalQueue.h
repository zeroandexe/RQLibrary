#pragma once

class RQQueue;
class RQSignal;
class RQNode;

/*
	带有信号量的队列，每一个
*/

class RQSignalQueue
{
public:
	//传入信号办
	RQSignalQueue();

	void push_front(RQNode*);
	void push_back(RQNode*);

	RQNode* pop_front();
	RQNode* pop_back();
	
	unsigned int GetCount();
private:
	RQQueue * _queue;
	RQSignal* _sig;						//信号量对象
};