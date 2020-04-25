#include "compiler/RQLine.h"
#include "define/RQInterDefine.h"

RQLine::RQLine()
{
	Create();
}


RQLine::~RQLine()
{
}

void RQLine::Create(void* param)
{
	Clear();
}

void RQLine::Clear()
{
	_lineNum = 0;
	_jumpUpLineNum = 0;
	_jumpDownLineNum = 0;
	_cscLabel = 0;
	_braceType = M_BRACE_TYPE_NOT;
}