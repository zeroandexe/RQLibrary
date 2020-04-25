#include "RQPair.h"

RQPair::RQPair()
{
	Init();
}


RQPair::~RQPair()
{
}

void RQPair::Create(void * param)
{
	Clear();
}

void  RQPair::Clear()
{
	_first._llong = _second._llong = 0;
}
