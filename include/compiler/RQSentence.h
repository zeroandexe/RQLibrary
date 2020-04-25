#pragma once
#include "node/RQNode.h"
#include "define/RQInterDefine.h"
#include "RQCodeSet.h"
#include "RQLine.h"

/*
	该对象是脚本中的语句对象，作为语句中子表达式的集合
	注意：
		集合中的所有元素来自于外界容器，所以这里并没有回收的权力，
		这里的回收仅仅是将集合的行调整到第一行，表示集合可以再次进行覆盖和访问
*/

class RQWord;			//元词对象
class RQVar;			//
class RQCodeSet;		//代码集合
class RQExpression;	//子表达式

class RQSentence : public RQLine
{
public:
	RQSentence();
	virtual ~RQSentence();

	virtual void Create(void* param = 0);
	virtual void Clear();

	//获取本行语句的标签
	int GetLabel();

	//设置本行语句的标签
	void SetLabel(int label);

	//向语句对象中添加一个解析表达式
	void AddExpression(RQExpression* expression);

	//重置获取行到第一条表达式
	void ResetHeadLine();

	//获取本行表达式的结局
	RQVar* GetResult();
	void SetResult(RQVar* result);

	//获取代码行中的表达式代码集合
	RQCodeSet* GetCodeSet();

private:
	RQCodeSet* _codeSet;
	RQVar* _result;			//整行语句结束后的计算结果
};