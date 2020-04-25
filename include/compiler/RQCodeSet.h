#pragma once
#include "node/RQNode.h"
#include "define/RQInterDefine.h"
/*
	该类是语句集合类，其中语句的含义可以是一行完整的代码的描述，
	也可以是一行代码中的原子表达式。
	本类主要是对代码流进行控制
	注意
		1、目前要求括号规定为
		if(){
		}
		else{
		}
		while(){
		}
		2、集合只负责对数据的存储，不负责对数据的释放，
*/

class RQLine;

class RQCodeSet : public RQNode
{
public:
	RQCodeSet();
	virtual ~RQCodeSet();

	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();
	//在当前行下标添加一个语句对象，并对代码块进行调整
	void AddLine(RQLine * line);

	//重置读取行到第一行语句对象
	void ResetHeadLine();

	//获取下一条有效语句对象，
	//若当前没有有效语句对象则返回空指针
	//若当前具有语句对象，则返回返回该对象，并将行下标+1
	RQLine* GetNextLine();

	//从当前位置向上跳转一个代码块（当前一定是‘}’的下一行）
	void JumpUpCodeBlock();

	//从当前位置向下跳转一个代码块（当前一定是‘{’）
	void JumpDownCodeBlock();

private:
	void _SetLineNum(int lineNum);
	int _pop();
	void _push(int lineNum);
private:
	RQLine * _lineArray[MAX_LINE_NUM_PER_SCRIPT];		//语句对象的存储数组
	unsigned int	_lineNumIndex;					//用于操作的行下标,起始下标为1
	unsigned int _LineCount;		//语句对象存在的总数
	int _lineNumberStack[32];		//花括号递归层次记录
	unsigned int _countStack;		//当前所在递归层次
};