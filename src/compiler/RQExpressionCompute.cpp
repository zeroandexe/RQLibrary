#include "compiler/RQExpressionCompute.h"
#include "define/RQInterDefine.h"
#include "RQVar.h"
#include "compiler/RQASTNode.h"
#include "define/RQASTDefine.h"
#include "compiler/RQExpression.h"
#include "compiler/RQVarCompute.h"

typedef RQExpressionCompute::Type_SymbolFun Type_SymbolFun;
Type_SymbolFun RQExpressionCompute::_symbolFunArray[M_SYMBOL_TOKEN_TOTAL];

RQExpressionCompute::RQExpressionCompute()
{
}


RQExpressionCompute::~RQExpressionCompute()
{
}

/*------------------�����������----------------------*/


void RQExpressionCompute::SymbolFun_Opt(RQExpression * expression)
{
	RQVarCompute::Compute(expression->_left, expression->_right, expression->_result, expression->_optToken);
}


void RQExpressionCompute::SymbolFun_Fun(RQExpression * expression)
{
	typedef  void(*Fun_Type)(RQVar**);
	((Fun_Type)expression->_funAddr->GetFun())(expression->_varArray);
}


//���}
void RQExpressionCompute::SymbolFun_BigRightParentheses(RQExpression * exepression) {
	
}

void RQExpressionCompute::Compute(RQExpression * exepression)
{
	(_symbolFunArray[exepression->_optToken])(exepression);
}


void RQExpressionCompute::InitObj()
{
	for (int i = 0; i < M_SYMBOL_TOKEN_TOTAL; i++)
	{
		_symbolFunArray[i] = &RQExpressionCompute::SymbolFun_Opt;
	}

	//��Ӻ�������
	_symbolFunArray[M_SYMBOL_TOKEN_FUN] = &RQExpressionCompute::SymbolFun_Fun;			//��Ӻ�������

	//����д�����
	_symbolFunArray[M_SYMBOL_TOKEN_BIG_RIGHT_PARENTHESES] = &SymbolFun_BigRightParentheses;

}