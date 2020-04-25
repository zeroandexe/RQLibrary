#include "compiler/RQCodeSet.h"
#include "compiler/RQLine.h"
#include "define/RQInterDefine.h"
#include "RecycleBin.h"

RQCodeSet::RQCodeSet()
{
	Clear();
}


RQCodeSet::~RQCodeSet()
{
}

void RQCodeSet::Create(void* param)
{
	Clear();
}

void * RQCodeSet::Init(void * param)
{
	
	return nullptr;
}

void RQCodeSet::Clear()
{
	_lineArray[1] = 0;
	_lineArray[0] = 0;
	_lineNumIndex = 1;
	_LineCount = 0;
	_countStack = 0;
}

//注意，目前假设左花括号不会单独出现，兼容语法后续再做
void RQCodeSet::AddLine(RQLine * line)
{
	int braceLeftLineNum;
	switch (line->_braceType)
	{
	case M_BRACE_TYPE_LEFT:					//检测当前语句是否具有左大括号
		_push(_lineNumIndex);				///由于当前语句带有左大括号，因此暂时无法获得右多括号进行匹配成完整的代码块，所以进行压栈待处理
		_lineArray[_lineNumIndex++] = line;		///记录当前穿行行
		break;
	case M_BRACE_TYPE_RIGHT:				//检测当前语句是否具有具有右大括号
		braceLeftLineNum = _pop();
		_lineArray[braceLeftLineNum]->_jumpDownLineNum = _lineNumIndex + 1;		///将左括号的下行跳转指到右括号的下一行
		line->_jumpUpLineNum = braceLeftLineNum;		///将右括号的上行跳转指到右括号的上一行
		_lineArray[_lineNumIndex++] = line;				///代码块已完成绑定，存入输入的语句对象
		break;
	default:								//当前语句不含有大括号
		_lineArray[_lineNumIndex++] = line;			///由于当前语句并不含花括号，所以当前语句没有分支，应当顺序记录
		break;
	}
	_lineArray[_lineNumIndex] = 0;
	_LineCount++;					///由于在数组中存入新的语句对象，语句对象数量统计+1
}

void RQCodeSet::ResetHeadLine()
{
	_lineNumIndex = 1;
}

RQLine * RQCodeSet::GetNextLine()
{
	RQLine* sentence = _lineArray[_lineNumIndex++];		///返回当前行下标所指的语句对象，并调整行下标到下一个有效语句
	if (!sentence)
	{
		_lineNumIndex--;			///调整行下标位置，防止下标被无限下行偏移
	}
	return sentence;
}

void RQCodeSet::JumpUpCodeBlock()
{
	_SetLineNum(_lineArray[_lineNumIndex - 1]->_jumpUpLineNum);		///调整上行代码块到执行部分
}

void RQCodeSet::JumpDownCodeBlock()
{
	_SetLineNum(_lineArray[_lineNumIndex - 1]->_jumpDownLineNum);		///调整下行代码块到执行行部分 
}


void RQCodeSet::_SetLineNum(int lineNum)
{
	_lineNumIndex = lineNum > 0 ? lineNum : 1;						///防止上越界
	_lineNumIndex = _lineNumIndex > _LineCount ? _LineCount : _lineNumIndex;		///防止下越界
}

int RQCodeSet::_pop()
{
	return _countStack ? _lineNumberStack[--_countStack] : 0;
}

void RQCodeSet::_push(int lineNum)
{
	_lineNumberStack[_countStack++] = lineNum;
}