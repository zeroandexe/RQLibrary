#pragma once
#include "node/RQNode.h"
#include "define/RQInterDefine.h"
#include "compiler/RQLine.h"
/*
	该对象是从词源分析后得带的一组表达式中的一种
*/

class RQVar;

class RQExpression : public RQLine
{
public:
	RQExpression();
	virtual ~RQExpression();

	virtual void Create(void* param = 0);
	virtual void Clear();

	RQVar* _left;							//运算符左参数
	RQVar* _right;							//运算符右参数
	RQVar* _result;							//运算结果
	RQVar* _funAddr;						//函数调用时函数地址
	int _optToken;							//运算符特征值
	RQVar* _varArray[MAX_PARAM_PER_FUN];	//函数参数值
};