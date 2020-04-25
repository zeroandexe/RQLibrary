#pragma once
#include "node/RQNode.h"

/*
	该类是CodeSet对象的一行，用于承载一个流程控制抽象层(CodeSet)中不可在分的原子量。
*/

class RQLine : public RQNode
{
public:
	RQLine();
	virtual ~RQLine();

	virtual void Create(void* param = 0);
	virtual void Clear();
	unsigned int _lineNum;			//行号
	unsigned int _jumpUpLineNum;	//向上跳跃的行号
	unsigned int _jumpDownLineNum;	//向下跳转的行号
	unsigned int _braceType;		//花括号类型
	unsigned int _cscLabel;			//行标签
};