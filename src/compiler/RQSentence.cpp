#include "compiler/RQSentence.h"
#include "RecycleBin.h"
#include "compiler/RQCodeSet.h"
#include "compiler/RQLine.h"

RQSentence::RQSentence()
{
	Create();
}


RQSentence::~RQSentence()
{
	RecycleBin::RecycleNode(_codeSet);
	Clear();
}

void RQSentence::Create(void* param)
{
	_codeSet = RecycleBin::GetFreeRQCodeSet();
}

void RQSentence::Clear()
{
	RQLine::Clear();
	_codeSet->Clear();		///不会集合本身，仅将其重置为初始化状态，用于下次继续使用
	_result = 0;
}

int RQSentence::GetLabel()
{
	return _cscLabel;
}

void RQSentence::SetLabel(int label)
{
	_cscLabel = label;
}

void RQSentence::AddExpression(RQExpression * expression)
{
	_codeSet->AddLine((RQLine*)expression);
}

void RQSentence::ResetHeadLine()
{
	_codeSet->ResetHeadLine();
}

RQVar * RQSentence::GetResult()
{
	return _result;
}

void RQSentence::SetResult(RQVar * result)
{
	_result = result;
}

RQCodeSet * RQSentence::GetCodeSet()
{
	return _codeSet;
}

