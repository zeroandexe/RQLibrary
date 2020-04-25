#include "RQQueue.h"

RQQueue::RQQueue(RQLineStorage* lineStorage)
{
	_lineStorage = lineStorage;
}


RQQueue::~RQQueue()
{
}

void RQQueue::push_front(RQNode * node)
{
	_lineStorage->push_front(node);
}

void RQQueue::push_back(RQNode * node)
{
	_lineStorage->push_back(node);
}

RQNode * RQQueue::pop_front()
{
	return _lineStorage->pop_front();
}

RQNode * RQQueue::pop_back()
{
	return _lineStorage->pop_back();
}

void RQQueue::Clear()
{
	_lineStorage->Clear();
}

RQLineStorage * RQQueue::GetStorage()
{
	return _lineStorage;
}

void RQQueue::Remove(RQNode * node)
{
	_lineStorage->Remove(node);
}

RQQueue*RQQueue::Create_list(RQLock* lock)
{
	return new RQQueue(new RQList(lock));
}

void RQQueue::Destroy(RQQueue* queue)
{
	RQLineStorage* lineStorage = queue->GetStorage();
	RQLock* lock = lineStorage->getLock();

	delete lock;
	delete lineStorage;
	delete queue;
}

unsigned int RQQueue::GetCount()
{
	return _lineStorage->GetCount();
}
