#include "RQSignalQueue.h"
#include "RQQueue.h"
#include "RQSignal.h"
#include "RQSpinLock.h"
#include <time.h>
#include <stdlib.h>

RQSignalQueue::RQSignalQueue()
{
	_sig = new RQSignal();
	_queue = RQQueue::Create_list(new RQSpinLock);
}

void RQSignalQueue::push_front(RQNode *node)
{
	_queue->push_front(node);
	_sig->Add();
}

void RQSignalQueue::push_back(RQNode * node)
{
	_queue->push_back(node);
	_sig->Add();
}


RQNode * RQSignalQueue::pop_front()
{
	_sig->Sub();
	return _queue->pop_front();
}


RQNode * RQSignalQueue::pop_back()
{
	_sig->Sub();
	return _queue->pop_back();
}


unsigned int RQSignalQueue::GetCount()
{
	return _queue->GetCount();
}
