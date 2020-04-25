#pragma once

/*
	code stream control，程序流控制器，这里简称CSC。用于处理if-else-while等带来的程序执行时分支跳转处理
*/

class RQLine;
class RQVar;

class RQCSC
{
public:
	RQCSC();
	typedef unsigned int(RQCSC::* Type_KeyLabelFun)(bool);			//声明关键标识处理函数类型
	virtual ~RQCSC();

	//传入控制流标签和当前语句的执行结果，返回语句行相对的跳转指令
	unsigned int Control(unsigned int cscLabel,bool result);

	//初始化程序流控制器函数调用模板
	static void InitObj();

	//重置程序流控制器对象内部数据
	void Reset();

private:
	int* _runStateArray;					//脚本if和while嵌套运行状态的记录数值
	int _runStateCount;						//在状态记录数组中当前状态的个数
	static Type_KeyLabelFun* _keyLabelFunArray;	//关键字函数数组

private:
	unsigned int KeyLabelFun_Invalid(bool result);
	unsigned int KeyLabelFun_If(bool result);
	unsigned int KeyLabelFun_While(bool result);
	unsigned int KeyLabelFun_Else(bool result);
	unsigned int KeyLabelFun_BigRightParentheses(bool result);

};