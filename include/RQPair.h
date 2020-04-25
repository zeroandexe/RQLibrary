#pragma once
#include "node/RQNode.h"
#include "RQPtrVal.h"

class RQPair : public RQNode
{
public:
	RQPair();
	~RQPair();
	virtual void Create(void* param = 0);
	virtual void Clear();
	RQPtrVal _first;
	RQPtrVal _second;
};