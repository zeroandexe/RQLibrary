#pragma once
#include <map>
#include "RQSymbolInfo.h"
#include "define/RQASTDefine.h"
/*
	该类是表达式计算器，属于解释器中的运算符执行器
*/

class RQASTNode;
class RQExpression;

class RQExpressionCompute
{
public:
	RQExpressionCompute();
	virtual ~RQExpressionCompute();
	typedef void(* Type_SymbolFun)(RQExpression* );				//声明运算符函数类型
	static void InitObj();

	/*
		表达式计算，根据传入的表达式的计算特征值进行计算。
	*/
	void Compute(RQExpression* exepression);

private:
						//创建运算符函数数组（采用运算符特征值作为函数元素索引）

	static std::map<std::string, RQSymbolInfo>* _symbolMap;

	static Type_SymbolFun _symbolFunArray[M_SYMBOL_TOKEN_TOTAL];
private:

	static void SymbolFun_Opt(RQExpression* expression);			// 其他操作
	static void SymbolFun_Fun(RQExpression* expression);			// function
	static void SymbolFun_BigRightParentheses(RQExpression* expression);	//	}
};