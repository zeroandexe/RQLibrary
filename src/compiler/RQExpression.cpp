#include "compiler/RQExpression.h"



RQExpression::RQExpression()
{
	Create();
}


RQExpression::~RQExpression()
{
}

void RQExpression::Create(void* param)
{
	Clear();
}

void RQExpression::Clear()
{
	RQLine::Clear();
	_left = 0;
	_right = 0;
	_result = 0;
	_funAddr = 0;
	_optToken = 0;
	_varArray[0] = 0;
}