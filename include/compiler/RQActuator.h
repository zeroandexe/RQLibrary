#pragma once
/*
	该类是编译器的执行器，用于三元址语句对象的执行处理
*/
class RQCSC;				//程序流控制器
class RQExpressionCompute;	//表达式结算期
class RQScriptFile;			//脚本文件对象
class RQSentence;
class RQCodeSet;			//代码集合
class RQExpression;
class RQVar;
class RQLine;

class RQActuator
{
public:
	RQActuator();
	~RQActuator();

	//传入执行脚本
	void RunScriptFile(RQScriptFile* scriptFile);	//执行传入的语句对象
private:

	unsigned int _RunSentence(RQLine* param);
	unsigned int _RunExpresion(RQLine* param);

	void _RunCodeSet(RQCodeSet* codeSet,RQCSC* csc,unsigned int(RQActuator::* Run)(RQLine*));
private:
	RQCSC * _sentenceCSC;							//脚本语句程序流控制器
	RQCSC* _expressionCSC;							//表达式语句程序流控制器
	RQExpressionCompute* _expressionCompute;		//表达式计算器
};