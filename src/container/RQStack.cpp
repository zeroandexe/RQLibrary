#include "RQStack.h"

RQStack::RQStack(RQLineStorage * lineStorage)
{
	_lineStorage = lineStorage;
}

RQStack::~RQStack()
{
}

void RQStack::push(RQNode * node)
{
	_lineStorage->push_back(node);
}

RQNode * RQStack::pop()
{
	return _lineStorage->pop_back();
}

void RQStack::Clear()
{
	_lineStorage->Clear();
}

RQLineStorage * RQStack::GetStorage()
{
	return _lineStorage;
}

RQStack * RQStack::Create_list(RQLock* lock)
{
	RQList* list = new RQList(lock);
	return new RQStack(list);
}


void RQStack::Destroy(RQStack * stack)
{
	RQLineStorage* lineStorage = stack->GetStorage();
	RQLock* lock = lineStorage->getLock();
	delete lock;
	delete lineStorage;
	delete stack;
}

unsigned int RQStack::GetCount()
{
	return _lineStorage->GetCount();
}
