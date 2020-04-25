#pragma once
#include <map>
#include "RQSymbolInfo.h"
#include "define/RQASTDefine.h"
/*
	�����Ǳ��ʽ�����������ڽ������е������ִ����
*/

class RQASTNode;
class RQExpression;

class RQExpressionCompute
{
public:
	RQExpressionCompute();
	virtual ~RQExpressionCompute();
	typedef void(* Type_SymbolFun)(RQExpression* );				//�����������������
	static void InitObj();

	/*
		���ʽ���㣬���ݴ���ı��ʽ�ļ�������ֵ���м��㡣
	*/
	void Compute(RQExpression* exepression);

private:
						//����������������飨�������������ֵ��Ϊ����Ԫ��������

	static std::map<std::string, RQSymbolInfo>* _symbolMap;

	static Type_SymbolFun _symbolFunArray[M_SYMBOL_TOKEN_TOTAL];
private:

	static void SymbolFun_Opt(RQExpression* expression);			// ��������
	static void SymbolFun_Fun(RQExpression* expression);			// function
	static void SymbolFun_BigRightParentheses(RQExpression* expression);	//	}
};